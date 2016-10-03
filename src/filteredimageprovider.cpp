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

#include "KCL/filteredimageprovider.h"

#include <QFile>
#include <QImageReader>
#include <QHash>

#include "KCL/filesystemutils.h"

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif

FilteredImageProvider::FilteredImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{

}

QImage FilteredImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int splitIndex = id.indexOf('@');

    QString url = splitIndex > -1 ? id.mid(splitIndex + 1) : id;
    QString paramsString = splitIndex > -1 ? id.left(splitIndex) : "";

    QString filename = FileSystemUtils::localPathFromUrl(url);

    QFile *imageDevice = new QFile(filename);
    imageDevice->open(QIODevice::ReadOnly);

    QImageReader reader;
    reader.setDecideFormatFromContent(true); // Important!
    reader.setDevice(imageDevice);

    QImage resultImage = reader.read();

    imageDevice->close();
    delete imageDevice;

    QStringList paramsList = paramsString.split(',');
    QHash<QString, QString> params;
    foreach (QString param, paramsList)
    {
        int splitIndex = param.indexOf('=');
        QString key = splitIndex > -1 ? param.left(splitIndex) : param;
        QString value = splitIndex > -1 ? param.mid(splitIndex + 1) : "";

        params.insert(key, value);
    }

    if (params.contains("invert"))
        resultImage.invertPixels();

    if (params.contains("scale"))
    {
        qreal scale = params.value("scale").toFloat();
        if (!qIsNull(scale))
        {
            resultImage = resultImage.scaled(resultImage.size() * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }

    if (size)
        *size = resultImage.size();

    return resultImage;
}
