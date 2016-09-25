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

#include "KCL/imageutils.h"

#include <QImage>
#include <QImageReader>

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

ImageUtils::ImageUtils(QObject *parent) :
    QObject(parent)
{
}

bool ImageUtils::checkParamsSameFormat(const QImage &srcImage, QImage &dstImage, bool modifyDst, bool zeroFillNew)
{
    if (srcImage.isNull())
        return false;

    if (dstImage.size() != srcImage.size())
    {
        if (modifyDst)
        {
            dstImage = QImage(srcImage.size(), QImage::Format_ARGB32);
            if (zeroFillNew) dstImage.fill(0);
        }
        else
            return false;
    }

    if (dstImage.format() != QImage::Format_ARGB32)
    {
        if (modifyDst)
            dstImage = dstImage.convertToFormat(QImage::Format_ARGB32);
        else
            return false;
    }

    return true;
}

bool ImageUtils::intensityToAlpha(const QImage &srcImage, QImage &dstImage)
{
    if (!checkParamsSameFormat(srcImage, dstImage, true, true))
        return false;

    for (int y = 0; y < dstImage.height(); ++y)
    {
        QRgb *s = (QRgb *)srcImage.constScanLine(y);
        QRgb *d = (QRgb *)dstImage.scanLine(y);

        for (int x = 0; x < dstImage.width(); ++x)
        {
            *d = (((qRed(*s) * 61 + qGreen(*s) * 174 + qBlue(*s) * 21) >> 8) << 24) | (*d & RGB_MASK);
            ++d; ++s;
        }
    }

    return true;
}

bool ImageUtils::minAlpha(const QImage &srcImage, QImage &dstImage)
{
    if (!checkParamsSameFormat(srcImage, dstImage, true, true))
        return false;

    for (int y = 0; y < dstImage.height(); ++y)
    {
        QRgb *s = (QRgb *)srcImage.constScanLine(y);
        QRgb *d = (QRgb *)dstImage.scanLine(y);

        for (int x = 0; x < dstImage.width(); ++x)
        {
            *d = (qMin(qAlpha(*s), qAlpha(*d)) << 24) | (*d & RGB_MASK);
            ++d; ++s;
        }
    }

    return true;
}

bool ImageUtils::maskedTransfer(const QImage &srcImage, QImage &dstImage, unsigned int mask)
{
    DGUARDMETHODTIMED;

    if (!checkParamsSameFormat(srcImage, dstImage, true, true))
        return false;

    for (int y = 0; y < dstImage.height(); ++y)
    {
        QRgb *s = (QRgb *)srcImage.constScanLine(y);
        QRgb *d = (QRgb *)dstImage.scanLine(y);

        for (int x = 0; x < dstImage.width(); ++x)
        {
            *d = *d | (*s & mask);
            ++d; ++s;
        }
    }

    return true;
}

bool ImageUtils::applyAlpha(const QImage &srcImage, QImage &dstImage, unsigned int alpha)
{
    DGUARDMETHODTIMED;

    if (!checkParamsSameFormat(srcImage, dstImage))
        return false;

    for (int y = 0; y < dstImage.height(); ++y)
    {
        QRgb *s = (QRgb *)srcImage.constScanLine(y);
        QRgb *d = (QRgb *)dstImage.scanLine(y);

        for (int x = 0; x < dstImage.width(); ++x)
        {
            *d = (*s & RGB_MASK) | ((qAlpha(*s) * alpha) >> 8) << 24;
            ++d; ++s;
        }
    }

    return true;
}

bool ImageUtils::hasAlphaValues(const QImage &srcImage, const QRect &srcRect)
{
    DGUARDMETHODTIMED;

    if (!srcImage.hasAlphaChannel())
        return false;

    bool result = false;

    QRect roi = srcRect.intersected(srcImage.rect());
    if (roi.isEmpty())
        roi = srcImage.rect();

    int roiBottom = roi.top() + roi.height();
    int roiRight = roi.left() + roi.width();

    if (srcImage.format() == QImage::Format_ARGB32)
    {
        for (int y = roi.top(); y < roiBottom; ++y)
        {
            QRgb *s = (QRgb *)srcImage.constScanLine(y) + roi.left();

            for (int x = roi.left(); x < roiRight; ++x)
            {
                const int alpha = qAlpha(*s);

                if (alpha < 0xFF)
                {
                    result = true;
                    break;
                }

                ++s;
            }
        }
    }
    else
    {
        for (int y = roi.top(); y < roiBottom; ++y)
        {
            for (int x = roi.left(); x < roiRight; ++x)
            {
                const int alpha = qAlpha(srcImage.pixel(x, y));

                if (alpha < 0xFF)
                {
                    result = true;
                    break;
                }
            }
        }
    }

    return result;
}

bool ImageUtils::convertToGrayscale(const QImage &srcImage, QImage &dstImage)
{
    if (srcImage.isNull())
        return false;

    QImage image = srcImage;
    if (image.format() != QImage::Format_ARGB32)
        image = image.convertToFormat(QImage::Format_ARGB32);

    dstImage = QImage(image.width(), image.height(), QImage::Format_Indexed8);

    QVector<QRgb> grayTable(256);
    for (int i = 0; i < 256; ++i)
        grayTable[i] = qRgb(i, i, i);

    dstImage.setColorTable(grayTable);

    int y;
    int height = image.height();
    int x;
    int width = image.width();
    register const QRgb *input;
    register uchar *output;

    for (y = 0; y < height; ++y)
    {
        input = (QRgb *)image.constScanLine(y);
        output = dstImage.scanLine(y);

        for (x = 0; x < width; ++x)
        {
            *output = qGray(*input);

            ++output;
            ++input;
        }
    }

    return true;
}

bool ImageUtils::imageFromVariant(const QVariant &image, QImage *result)
{
    if (image.canConvert<QImage>())
    {
        if (result)
            *result = image.value<QImage>();

        return true;
    }

    return false;
}

QVariant ImageUtils::empty(const QSize &size)
{
    return QImage(size, QImage::Format_ARGB32);
}

QVariant ImageUtils::load(const QString &fileName, const QSize &requestedSize, bool returnExactSize)
{
    QImageReader reader;
    reader.setFileName(fileName);

    if (returnExactSize)
    {
        QSize fittedSize(reader.size());
        fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
        reader.setScaledSize(fittedSize);
    }

    QImage result = reader.read();
    if (result.isNull())
    {
        reader.setDecideFormatFromContent(true);
        reader.setFileName(fileName);

        result = reader.read();

        if (result.isNull())
        {
            qDebug("ImageUtils::load: %s", reader.errorString().toLatin1().constData());
            return QVariant(false);
        }
    }

    return result;
}

bool ImageUtils::save(const QVariant &image, const QString &fileName, const QString &format, int quality)
{
    QImage img;
    if (imageFromVariant(image, &img) && !img.isNull())
        return img.save(fileName, format.isNull() ? 0 : format.toLatin1().constData(), quality);

    return false;
}

QVariant ImageUtils::scaled(const QVariant &srcImage, const QSize &dstSize, int aspectMode, int mode)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.scaled(dstSize, (Qt::AspectRatioMode)aspectMode, (Qt::TransformationMode)mode);

    return QVariant(false);
}

QVariant ImageUtils::copy(const QVariant &srcImage, const QRect &srcRect)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.copy(srcRect);

    return QVariant(false);
}

QVariant ImageUtils::fill(const QVariant &targetImage, const QColor &color)
{
    QImage img;
    if (imageFromVariant(targetImage, &img) && !img.isNull())
    {
        img.fill(color.rgba());
        return img;
    }

    return QVariant(false);
}

QVariant ImageUtils::invert(const QVariant &targetImage, bool invertAlphaToo)
{
    QImage img;
    if (imageFromVariant(targetImage, &img) && !img.isNull())
    {
        img.invertPixels(invertAlphaToo ? QImage::InvertRgba : QImage::InvertRgb);
        return img;
    }

    return QVariant(false);
}

QVariant ImageUtils::maskedTransfer(const QVariant &srcImage, const QVariant &dstImage, quint32 mask)
{
    QImage src;
    QImage dst;
    if (imageFromVariant(srcImage, &src) && !src.isNull())
    {
        imageFromVariant(dstImage, &dst);
        if (maskedTransfer(src, dst, mask))
            return dst;
    }

    return QVariant(false);
}

QVariant ImageUtils::intensityToAlpha(const QVariant &srcImage, const QVariant &dstImage)
{
    QImage src;
    QImage dst;
    if (imageFromVariant(srcImage, &src) && !src.isNull())
    {
        imageFromVariant(dstImage, &dst);
        if (intensityToAlpha(src, dst))
            return dst;
    }

    return QVariant(false);
}

QVariant ImageUtils::minAlpha(const QVariant &srcImage, const QVariant &dstImage)
{
    QImage src;
    QImage dst;
    if (imageFromVariant(srcImage, &src) && !src.isNull())
    {
        imageFromVariant(dstImage, &dst);
        if (minAlpha(src, dst))
            return dst;
    }

    return QVariant(false);
}

QVariant ImageUtils::applyAlpha(const QVariant &srcImage, const QVariant &dstImage, quint32 alpha)
{
    QImage src;
    QImage dst;
    if (imageFromVariant(srcImage, &src) && !src.isNull())
    {
        imageFromVariant(dstImage, &dst);
        if (applyAlpha(src, dst, alpha))
            return dst;
    }

    return QVariant(false);
}

QVariant ImageUtils::size(const QVariant &srcImage)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.size();

    return QSize(0, 0);
}
