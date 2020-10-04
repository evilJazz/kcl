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

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSize>
#include <QRect>
#include <QVariant>
#include <QColor>

#ifdef KCL_filesystemutils
#include "filesystemutils.h"
#endif

class KCL_EXPORT ImageUtils : public QObject
{
    Q_OBJECT
public:
    explicit ImageUtils(QObject *parent = 0);

    static bool checkParamsSameFormat(const QImage &srcImage, QImage &dstImage, bool modifyDst = true, bool zeroFillNew = false);

    static bool intensityToAlpha(const QImage &srcImage, QImage &dstImage);
    static bool minAlpha(const QImage &srcImage, QImage &dstImage);
    static bool maskedTransfer(const QImage &srcImage, QImage &dstImage, unsigned int mask);
    static bool applyAlpha(const QImage &srcImage, QImage &dstImage, unsigned int alpha);
    static bool hasAlphaValues(const QImage &srcImage, const QRect &srcRect = QRect());

    static bool convertToGrayscale(const QImage &srcImage, QImage &dstImage);

    static bool imageFromVariant(const QVariant &image, QImage *result = NULL);

    /* Image methods for use in JavaScript - all require QImage::Format_ARGB32 */

    Q_INVOKABLE static QVariant empty(const QSize &size);

    static QVariant load(QIODevice *device, const QSize &requestedSize = QSize(), bool returnExactSize = false);
    static bool save(const QVariant &image, QIODevice *device, const QString &format = QString::null, int quality = -1);

    Q_INVOKABLE static QVariant load(const QString &fileName, const QSize &requestedSize = QSize(), bool returnExactSize = false);
    Q_INVOKABLE static bool save(const QVariant &image, const QString &fileName, const QString &format = QString::null, int quality = -1);

#ifdef KCL_filesystemutils
    Q_INVOKABLE static QVariant load(IODevice *device, const QSize &requestedSize = QSize(), bool returnExactSize = false);
    Q_INVOKABLE bool save(const QVariant &image, IODevice *device, const QString &format, int quality);
#endif

    Q_INVOKABLE static QVariant scaled(const QVariant &srcImage, const QSize &dstSize, int aspectMode = Qt::IgnoreAspectRatio, int mode = Qt::FastTransformation);
    Q_INVOKABLE static QVariant copy(const QVariant &srcImage, const QRect &srcRect);

    Q_INVOKABLE static QVariant fill(const QVariant &targetImage, const QColor &color);
    Q_INVOKABLE static QVariant invert(const QVariant &targetImage, bool invertAlphaToo = false);

    Q_INVOKABLE static QVariant maskedTransfer(const QVariant &srcImage, const QVariant &dstImage, quint32 mask);
    Q_INVOKABLE static QVariant intensityToAlpha(const QVariant &srcImage, const QVariant &dstImage);
    Q_INVOKABLE static QVariant minAlpha(const QVariant &srcImage, const QVariant &dstImage);
    Q_INVOKABLE static QVariant applyAlpha(const QVariant &srcImage, const QVariant &dstImage, quint32 alpha);

    Q_INVOKABLE static QVariant size(const QVariant &srcImage);

signals:

public slots:

};

#endif // IMAGEUTILS_H
