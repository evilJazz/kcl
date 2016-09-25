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

#include "KCL/colorutils.h"

#include "KCL/imageutils.h"

#include <QImage>

#ifdef KCL_WIDGETS
#include <QPixmap>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#endif
#endif

ColorUtils::ColorUtils(QObject *parent) :
    QObject(parent)
{
}

QVariantMap ColorUtils::colorComponents(const QColor &color, bool asFloat)
{
    QVariantMap result;

    result.insert("isValid", color.isValid());

    if (asFloat)
    {
        result.insert("a", color.alphaF());
        result.insert("r", color.redF());
        result.insert("g", color.greenF());
        result.insert("b", color.blueF());
    }
    else
    {
        result.insert("a", color.alpha());
        result.insert("r", color.red());
        result.insert("g", color.green());
        result.insert("b", color.blue());
    }

    return result;
}

QString ColorUtils::colorName(const QColor &color)
{
    return QString().sprintf("#%08X", color.rgba());
}

QColor ColorUtils::parseColor(const QString &colorString)
{
    if (colorString.startsWith("#") && colorString.length() == 7)
    {
        int red = ("0x" + colorString.mid(1, 2)).toUShort(0, 16);
        int green = ("0x" + colorString.mid(3, 2)).toUShort(0, 16);
        int blue = ("0x" + colorString.mid(5, 2)).toUShort(0, 16);

        return QColor(red, green, blue);
    }
    else if (colorString.startsWith("#") && colorString.length() == 9)
    {
        int alpha = ("0x" + colorString.mid(1, 2)).toUShort(0, 16);
        int red = ("0x" + colorString.mid(3, 2)).toUShort(0, 16);
        int green = ("0x" + colorString.mid(5, 2)).toUShort(0, 16);
        int blue = ("0x" + colorString.mid(7, 2)).toUShort(0, 16);

        return QColor(red, green, blue, alpha);
    }
    else
        return QColor(colorString);
}

QColor ColorUtils::setAlpha(const QColor &color, QVariant alpha)
{
    QColor copy = color;

    if (alpha.type() == QVariant::Double)
        copy.setAlphaF(alpha.toDouble());
    else
        copy.setAlpha(alpha.toInt());

    return copy;
}

QColor ColorUtils::grabColorFromScreen(const QPoint &screenPos)
{
#ifdef KCL_WIDGETS
    QPoint p = screenPos;
    if (p == QPoint(INT_MAX, INT_MAX))
        p = QCursor::pos();

    const QDesktopWidget *desktop = QApplication::desktop();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(), p.x(), p.y(), 1, 1);
    QImage i = pixmap.toImage();
#else
    const QPixmap pixmap = QPixmap::grabWindow(desktop->winId(), p.x(), p.y(), 1, 1);
    QImage i = pixmap.toImage();
#endif

    return i.pixel(0, 0);
#else
    qWarning("ColorUtils::grabColorFromScreen is not implemented.");
    return QColor();
#endif
}

QColor ColorUtils::grabColorFromImage(const QVariant image, int posX, int posY)
{
    QImage src;
    if (ImageUtils::imageFromVariant(image, &src) && !src.isNull())
        return src.pixel(posX, posY);
    else
        return QColor();
}
