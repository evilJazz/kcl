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

#ifndef COLORUTILS_H
#define COLORUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QColor>
#include <QPoint>

#ifdef Q_OS_SYMBIAN
// Workaround to avoid naming clash with Symbian RTL
#define ColorUtils KCLColorUtils
#endif

class KCL_EXPORT ColorUtils : public QObject
{
    Q_OBJECT
public:
    explicit ColorUtils(QObject *parent = 0);

    Q_INVOKABLE static QVariantMap colorComponents(const QColor &color, bool asFloat = false);
    Q_INVOKABLE static QString colorName(const QColor &color);
    Q_INVOKABLE static QColor parseColor(const QString &colorString);
    Q_INVOKABLE static QColor setAlpha(const QColor &color, QVariant alpha);

    Q_INVOKABLE static QColor grabColorFromScreen(const QPoint &screenPos = QPoint(INT_MAX, INT_MAX));
    Q_INVOKABLE static QColor grabColorFromImage(const QVariant image, int posX = 0, int posY = 0);

signals:

public slots:

};

#endif // COLORUTILS_H
