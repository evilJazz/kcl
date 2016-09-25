/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

#include "KCL/imagefastloader.h"

#include <QtAlgorithms>
#include <QImage>
#include <QImageReader>
#include <QRgb>
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QCryptographicHash>

#include "KCL/filesystemutils.h"

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

#define CBCI_MARKER "CBCIV3"
#define CBCI_MARKER_SIZE (sizeof(CBCI_MARKER) - 1)

bool megaPixelLessThan(const CachedImageSize &s1, const CachedImageSize &s2)
{
    return (s1.width * s1.height) < (s2.width * s2.height);
}

/* DiskImageCache */

DiskImageCache::DiskImageCache(QObject *parent)
    :   QObject(parent),
        cacheDirectory_()
{
}

void DiskImageCache::clearCacheDirectory()
{
    if (!cacheDirectory_.isEmpty() && cacheDirectory_.length() > 3 && QDir(cacheDirectory_).exists())
    {
        FileSystemUtils::removeDirectoryRecursively(cacheDirectory_);
    }
}

QString DiskImageCache::getFilenameForKey(const QString &key) const
{
    if (cacheDirectory_.isEmpty())
        return QString::null;
    else
        return cacheDirectory_ + '/' + key + ".cbci";
}

bool DiskImageCache::isImageCached(const QString &key) const
{
    return QFile(getFilenameForKey(key)).exists();
}

DiskImageCache::ImageCacheResult DiskImageCache::saveImage(const QString &key, QIODevice *srcImageStream)
{
    DGUARDMETHODTIMED;

    QImage srcImage;
    if (!srcImage.load(srcImageStream, 0))
        return LoadError;

    return saveImage(key, &srcImage);
}

DiskImageCache::ImageCacheResult DiskImageCache::saveImage(const QString &key, QImage *srcImage)
{
    DGUARDMETHODTIMED;

    QString filename = getFilenameForKey(key);
    if (filename.isEmpty())
        return KeyInvalid;

    QDir dir = QFileInfo(filename).absoluteDir();
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    QFile file(filename);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    return saveCacheImages(srcImage, imageSizes_, &file) ? NoError : SaveError;
}

QSize DiskImageCache::getOriginalImageSizeFromImage(const QString &key)
{
    QString filename = getFilenameForKey(key);
    if (filename.isEmpty())
        return QSize();

    if (!isImageCached(key))
        return QSize();

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    return readCacheOriginalImageSize(&file);
}

DiskImageCache::ImageCacheResult DiskImageCache::loadImage(const QString &key, QImage *dstImage, const QSize &requestedSize, bool returnExactSize, QSize *originalSize, QList<CachedImageSize> *availableImageSizesInCache)
{
    DGUARDMETHODTIMED;

    QString filename = getFilenameForKey(key);
    if (filename.isEmpty())
        return KeyInvalid;

    if (!isImageCached(key))
        return KeyDoesNotExist;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    return loadCacheImage(&file, requestedSize, dstImage, returnExactSize, originalSize, availableImageSizesInCache);
}

bool DiskImageCache::saveCacheImages(QImage *srcImage, const QList<QSize> &sizes, QIODevice *dstStream)
{
    DGUARDMETHODTIMED;

    QList<CachedImageSize> cacheImageSizes;

    for (int i = 0; i < sizes.count(); ++i)
    {
        CachedImageSize imageSize;

        QSize size(srcImage->size());
        size.scale(sizes[i], Qt::KeepAspectRatio);

        imageSize.width = size.width();
        imageSize.height = size.height();
        cacheImageSizes.append(imageSize);
    }

    qSort(cacheImageSizes.begin(), cacheImageSizes.end(), megaPixelLessThan);

    qint32 lastPos = 0;

    for (int i = 0; i < cacheImageSizes.count(); ++i)
    {
        CachedImageSize &imageSize = cacheImageSizes[i];
        imageSize.offset = dstStream->pos();

        QImage newImage = srcImage->scaled(imageSize.width, imageSize.height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        newImage.save(dstStream, "JPEG", 88);

        imageSize.count = dstStream->pos() - lastPos;
        lastPos = dstStream->pos();

        if (newImage.hasAlphaChannel())
        {
            QImage alphaImage = newImage.alphaChannel();
            alphaImage.save(dstStream, "JPEG", 88);
            imageSize.maskCount = dstStream->pos() - lastPos;
            lastPos = dstStream->pos();
        }
        else
            imageSize.maskCount = 0;
    }

    // append the original image size
    CachedImageSize originalImageSize;
    originalImageSize.width = srcImage->width();
    originalImageSize.height = srcImage->height();
    originalImageSize.count = 0;
    originalImageSize.maskCount = 0;
    originalImageSize.offset = -1;
    cacheImageSizes.append(originalImageSize);

    lastPos = dstStream->pos();

    qint32 temp = cacheImageSizes.count();
    dstStream->write((const char *)&temp, sizeof(qint32));
    for (int i = 0; i < cacheImageSizes.count(); ++i)
    {
        CachedImageSize &imageSize = cacheImageSizes[i];
        dstStream->write((const char *)&imageSize.width, sizeof(qint32));
        dstStream->write((const char *)&imageSize.height, sizeof(qint32));
        dstStream->write((const char *)&imageSize.offset, sizeof(qint32));
        dstStream->write((const char *)&imageSize.count, sizeof(qint32));
        dstStream->write((const char *)&imageSize.maskCount, sizeof(qint32));
    }

    dstStream->write((const char *)&lastPos, sizeof(qint32));
    dstStream->write(CBCI_MARKER, CBCI_MARKER_SIZE);

    return true;
}

QList<CachedImageSize> DiskImageCache::readCacheImageSizes(QIODevice *srcStream)
{
    DGUARDMETHODTIMED;

    QList<CachedImageSize> cacheImageSizes;

    srcStream->seek(srcStream->size() - CBCI_MARKER_SIZE);

    QByteArray marker = srcStream->read(CBCI_MARKER_SIZE);

    if (marker.indexOf(CBCI_MARKER) == 0)
    {
        srcStream->seek(srcStream->size() - CBCI_MARKER_SIZE - sizeof(qint32));

        qint32 offsetTable;
        srcStream->read((char *)&offsetTable, sizeof(qint32));
        srcStream->seek(offsetTable);

        qint32 offsetTableCount;
        srcStream->read((char *)&offsetTableCount, sizeof(qint32));

        for (int i = 0; i < offsetTableCount; ++i)
        {
            CachedImageSize imageSize;
            srcStream->read((char *)&imageSize.width, sizeof(qint32));
            srcStream->read((char *)&imageSize.height, sizeof(qint32));
            srcStream->read((char *)&imageSize.offset, sizeof(qint32));
            srcStream->read((char *)&imageSize.count, sizeof(qint32));
            srcStream->read((char *)&imageSize.maskCount, sizeof(qint32));
            cacheImageSizes.append(imageSize);
        }
    }

    return cacheImageSizes;
}

QSize DiskImageCache::readCacheOriginalImageSize(QIODevice *srcStream)
{
    srcStream->seek(srcStream->size() - CBCI_MARKER_SIZE);

    QByteArray marker = srcStream->read(CBCI_MARKER_SIZE);

    if (marker.indexOf(CBCI_MARKER) == 0)
    {
        srcStream->seek(srcStream->size() - CBCI_MARKER_SIZE - 6 * sizeof(qint32));

        qint32 width;
        qint32 height;

        srcStream->read((char *)&width, sizeof(qint32));
        srcStream->read((char *)&height, sizeof(qint32));

        return QSize(width, height);
    }
    else
        return QSize();
}

int DiskImageCache::findBestCacheImageMatch(const QList<CachedImageSize> &cacheImageSizes, const QSize &requestedSize)
{
    DGUARDMETHODTIMED;

    int cacheImageIndex = -1;

    if (cacheImageSizes.count() > 1)
    {
        const CachedImageSize &originalSize = cacheImageSizes.last();
        if (originalSize.width * originalSize.height == 0)
            return -1;

        QSize fittedSize(originalSize.width, originalSize.height);
        fittedSize.scale(requestedSize, Qt::KeepAspectRatio);

        int fittedMegaPixels = fittedSize.width() * fittedSize.height();

        for (int i = 0; i < cacheImageSizes.count() - 1; ++i)
        {
            const CachedImageSize &size = cacheImageSizes[i];
            if (fittedMegaPixels <= size.width * size.height)
            {
                cacheImageIndex = i;
                break;
            }
        }
    }

    return cacheImageIndex;
}

DiskImageCache::ImageCacheResult DiskImageCache::loadCacheImage(QIODevice *srcStream, const CachedImageSize &imageSize, QImage *dstImage)
{
    DGUARDMETHODTIMED;

    if (imageSize.offset == -1 || imageSize.count == 0 || !srcStream->seek(imageSize.offset))
        return LoadError;

    QByteArray data = srcStream->read(imageSize.count);
    if (data.size() == 0 || !dstImage->loadFromData(data, "JPEG"))
        return LoadError;

    if (imageSize.maskCount > 0)
    {
        QImage alphaImage;

        data = srcStream->read(imageSize.maskCount);
        if (data.size() == 0 || !alphaImage.loadFromData(data, "JPEG"))
            return LoadError;

        dstImage->setAlphaChannel(alphaImage);
    }

    return NoError;
}

DiskImageCache::ImageCacheResult DiskImageCache::loadCacheImage(
    QIODevice *srcStream, const QSize &requestedSize, QImage *dstImage, bool returnExactSize, QSize *originalSize, QList<CachedImageSize> *availableImageSizesInCache)
{
    DGUARDMETHODTIMED;

    QList<CachedImageSize> cacheImageSizes = readCacheImageSizes(srcStream);

    if (availableImageSizesInCache)
        *availableImageSizesInCache = cacheImageSizes;

    if (cacheImageSizes.count() == 0)
        return LoadError;

    int bestMatchIndex = findBestCacheImageMatch(cacheImageSizes, requestedSize);

    if (originalSize)
    {
        CachedImageSize &imageSize = cacheImageSizes.last();
        *originalSize = QSize(imageSize.width, imageSize.height);
    }

    if (bestMatchIndex > -1)
    {
        DiskImageCache::ImageCacheResult result = loadCacheImage(srcStream, cacheImageSizes[bestMatchIndex], dstImage);
        if (result == NoError && returnExactSize)
        {
            QSize fittedSize(dstImage->size());
            fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
            *dstImage = dstImage->scaled(fittedSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        return result;
    }
    else
        return RequestedSizeNotInCache;
}

/* ImageFastLoader */

QSize ImageFastLoader::getOriginalImageSizeFromImage(const QString &filename)
{
    QFileInfo srcFileInfo(filename);

    if (!srcFileInfo.exists())
        return QSize();

    QFileInfo cachedFileInfo(getFilenameForKey(filename));

    QSize result;

    if (cachedFileInfo.exists())
        result = DiskImageCache::getOriginalImageSizeFromImage(filename);

    if (result.isEmpty())
    {
        QImageReader reader;
        reader.setDecideFormatFromContent(true); // Important!
        reader.setFileName(filename);
        result = reader.size();
    }

    return result;
}

DiskImageCache::ImageCacheResult ImageFastLoader::loadImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize, QSize *originalSize, QList<CachedImageSize> *availableImageSizesInCache)
{
    QSize originalImageSize;
    DiskImageCache::ImageCacheResult result = DiskImageCache::loadImage(filename, dstImage, requestedSize, returnExactSize, &originalImageSize, availableImageSizesInCache);
    if (originalSize)
        *originalSize = originalImageSize;

    if (result == DiskImageCache::RequestedSizeNotInCache)
    {
        QImageReader reader;
        reader.setDecideFormatFromContent(true); // Important!
        reader.setFileName(filename);
        if (returnExactSize)
        {
            QSize fittedSize(originalImageSize);
            fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
            reader.setScaledSize(fittedSize);
        }

        result = reader.read(dstImage) ? DiskImageCache::NoError : DiskImageCache::LoadError;
    }

    return result;
}

DiskImageCache::ImageCacheResult ImageFastLoader::getImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize, QSize *originalSize, QList<CachedImageSize> *availableImageSizesInCache)
{
    QFileInfo srcFileInfo(filename);

    if (!srcFileInfo.exists())
        return DiskImageCache::KeyDoesNotExist;

    DiskImageCache::ImageCacheResult result;

    QFileInfo cachedFileInfo(getFilenameForKey(filename));

    // Cache file non-existent or too old?
    if (!cachedFileInfo.exists() ||
        srcFileInfo.lastModified() > cachedFileInfo.lastModified() ||
        srcFileInfo.created() > cachedFileInfo.created())
    {
        result = createImage(filename);
        if (result != NoError)
            return result;
    }

    result = loadImage(filename, dstImage, requestedSize, returnExactSize, originalSize, availableImageSizesInCache);

    // Invalid file?
    if (result == DiskImageCache::LoadError)
    {
        result = createImage(filename);
        if (result == NoError)
            result = loadImage(filename, dstImage, requestedSize, returnExactSize, originalSize, availableImageSizesInCache);
    }

    return result;
}

DiskImageCache::ImageCacheResult ImageFastLoader::createImage(const QString &filename)
{
    DiskImageCache::ImageCacheResult result;
    QFile srcFile(filename);
    srcFile.open(QIODevice::ReadOnly);
    result = saveImage(filename, &srcFile);
    srcFile.close();
    return result;
}

QString ImageFastLoader::createCacheKeyForFileName(const QString &filename) const
{
    QCryptographicHash crypto(QCryptographicHash::Md5);
    crypto.addData(filename.toUtf8());
    QString md5Sum = crypto.result().toHex();
    return md5Sum.left(2) + "/" + md5Sum.mid(2, 2) + "/" + md5Sum.mid(4, 2) + "/" + md5Sum.mid(6);
}

QString ImageFastLoader::getFilenameForKey(const QString &key) const
{
    if (cacheDirectory().isEmpty())
        return key + ".cbci";
    else
        return cacheDirectory() + "/" + (createSubdirectoriesInCacheDirectory_ ? createCacheKeyForFileName(key) : QFileInfo(key).fileName()) + ".cbci";
}
