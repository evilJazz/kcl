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

#include "KCL/graphicsutils.h"

#include <QImage>
#include <QImageReader>

GraphicsUtils::GraphicsUtils(QObject *parent) :
    QObject(parent)
{
}

QSize GraphicsUtils::scaledSize(QSize srcSize, const QSize &dstSize, int aspectMode)
{
    srcSize.scale(dstSize, (Qt::AspectRatioMode)aspectMode);
    return srcSize;
}

QSizeF GraphicsUtils::scaledSize(QSizeF srcSize, const QSizeF &dstSize, int aspectMode)
{
    srcSize.scale(dstSize, (Qt::AspectRatioMode)aspectMode);
    return srcSize;
}

QSize GraphicsUtils::scaleSize(const QSize &srcSize, qreal scale)
{
    return srcSize * scale;
}

QSizeF GraphicsUtils::scaleSize(const QSizeF &srcSize, qreal scale)
{
    return srcSize * scale;
}

QRect GraphicsUtils::scaleRect(const QRect &srcRect, qreal scale)
{
    return QRect(srcRect.x() * scale, srcRect.y() * scale, srcRect.width() * scale, srcRect.height() * scale);
}

QRectF GraphicsUtils::scaleRect(const QRectF &srcRect, qreal scale)
{
    return QRect(srcRect.x() * scale, srcRect.y() * scale, srcRect.width() * scale, srcRect.height() * scale);
}
