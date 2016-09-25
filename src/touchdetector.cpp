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

#include "../include/KCL/touchdetector.h"

#ifdef KCL_QTQUICK2
#include <QCursor>
#define QDeclarativeItem QQuickItem
#endif

TouchDetector::TouchDetector(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    touchDeviceActive_(false)
{
#ifdef KCL_QTQUICK2
    setAcceptedMouseButtons(Qt::AllButtons);
#endif
}

TouchDetector::~TouchDetector()
{
}

bool TouchDetector::touchDeviceActive() const
{
    return touchDeviceActive_;
}

#ifdef KCL_QTQUICK2
void TouchDetector::touchEvent(QTouchEvent *event)
{
    setTouchDeviceActive(true);
    event->ignore();
    QDeclarativeItem::touchEvent(event);
}

void TouchDetector::mousePressEvent(QMouseEvent *event)
{
    if (event->source() == Qt::MouseEventNotSynthesized)
        setTouchDeviceActive(false);

    event->ignore();
    QDeclarativeItem::mousePressEvent(event);
}

void TouchDetector::mouseMoveEvent(QMouseEvent *event)
{
    if (event->source() == Qt::MouseEventNotSynthesized)
        setTouchDeviceActive(false);

    event->ignore();
    QDeclarativeItem::mouseMoveEvent(event);
}

void TouchDetector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->source() == Qt::MouseEventNotSynthesized)
        setTouchDeviceActive(false);

    event->ignore();
    QDeclarativeItem::mouseReleaseEvent(event);
}

void TouchDetector::wheelEvent(QWheelEvent *event)
{
    setTouchDeviceActive(false);
    event->ignore();
    QDeclarativeItem::wheelEvent(event);
}

#endif

void TouchDetector::setTouchDeviceActive(bool value)
{
    if (touchDeviceActive_ != value)
    {
        touchDeviceActive_ = value;
        emit touchDeviceActiveChanged();
    }
}
