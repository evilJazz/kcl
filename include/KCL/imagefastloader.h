/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifndef IMAGEFASTLOADER_H
#define IMAGEFASTLOADER_H

#include "KCL/kcl_global.h"

#include <QIODevice>
#include <QList>
#include <QSize>
#include <QImage>

namespace KCL {

class KCL_EXPORT CachedImageSize
{
public:
    CachedImageSize() : width(0), height(0), offset(0), count(0), maskCount(0) {}
    qint32 width;
    qint32 height;
    qint32 offset;
    qint32 count;
    qint32 maskCount;
};

class KCL_EXPORT DiskImageCache : public QObject
{
    Q_OBJECT
public:
    enum ImageCacheResult
    {
        NoError = 0,
        LoadError = 1,
        SaveError = 2,
        RequestedSizeNotInCache = 3,
        KeyInvalid = 4,
        KeyDoesNotExist = 5
    };

    DiskImageCache(QObject *parent = 0);

    QString cacheDirectory() const { return cacheDirectory_; }
    void setCacheDirectory(const QString &dirName) { cacheDirectory_ = dirName; }

    virtual void clearCacheDirectory();

    QList<QSize> &imageSizes() { return imageSizes_; }
    virtual bool isImageCached(const QString &key) const;

    virtual ImageCacheResult saveImage(const QString &key, QImage *srcImage);
    virtual ImageCacheResult saveImage(const QString &key, QIODevice *srcImageStream);
    virtual QSize getOriginalImageSizeFromImage(const QString &key);
    virtual ImageCacheResult loadImage(const QString &key, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);

public:
    static bool saveCacheImages(QImage *srcImage, const QList<QSize> &sizes, QIODevice *dstStream);
    static QList<CachedImageSize> readCacheImageSizes(QIODevice *srcStream);
    static QSize readCacheOriginalImageSize(QIODevice *srcStream);
    static int findBestCacheImageMatch(const QList<CachedImageSize> &cacheImageSizes, const QSize &requestedSize);
    static ImageCacheResult loadCacheImage(QIODevice *srcStream, const CachedImageSize &imageSize, QImage *dstImage);
    static ImageCacheResult loadCacheImage(QIODevice *srcStream, const QSize &requestedSize, QImage *dstImage, bool returnExactSize = false, QSize *originalSize = NULL);

protected:
    virtual QString getFilenameForKey(const QString &key) const;

private:
    QString cacheDirectory_;
    QList<QSize> imageSizes_;
};

class KCL_EXPORT ImageFastLoader : public DiskImageCache
{
    Q_OBJECT
public:
    ImageFastLoader(QObject *parent = 0) : DiskImageCache(parent) {}

    virtual QSize getOriginalImageSizeFromImage(const QString &filename);
    virtual DiskImageCache::ImageCacheResult loadImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);
    virtual DiskImageCache::ImageCacheResult getImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);

protected:
    virtual QString getFilenameForKey(const QString &key) const;
    virtual DiskImageCache::ImageCacheResult createImage(const QString &filename);

private:
    QString createCacheKeyForFileName(const QString &filename) const;
};

}

#endif // IMAGEFASTLOADER_H
