/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf, Wincent Balin
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

#include "KCL/base64imageprovider.h"

#include <QBuffer>
#include <QImageReader>

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif


Base64ImageProvider::Base64ImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage Base64ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QByteArray imageData = QByteArray::fromBase64(id.toLatin1());
    QBuffer *imageDevice = new QBuffer(&imageData);
    imageDevice->open(QIODevice::ReadOnly);

    QImageReader reader;
    reader.setDecideFormatFromContent(true); // Important!
    reader.setDevice(imageDevice);
    QImage originalImage = reader.read();
    QSize originalSize = originalImage.size();
    imageDevice->close();

    QImage resultImage;
    if (originalSize == requestedSize)
    {
        if (size)
            *size = originalSize;

        resultImage = originalImage;
    }
    else
    {
        QSize fittedSize(originalSize);
        fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
        *size = fittedSize;

        imageDevice->open(QIODevice::ReadOnly);
        reader.setDevice(imageDevice);
        reader.setScaledSize(fittedSize);
        resultImage = reader.read();
        imageDevice->close();
    }

    delete imageDevice;

    return resultImage;
}
