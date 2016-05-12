/***************************************************************************
 *   Copyright (C) 2011-2016 Andre Beckedorf                               *
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

#ifndef TOUCHDETECTOR_H
#define TOUCHDETECTOR_H

#include "KCL/kcl_global.h"

#ifdef KCL_QTQUICK2
    #include <QQuickItem>
    #include <QQmlComponent>
    #include <QMouseEvent>
#else
    #include <QDeclarativeItem>
#endif

#include <QTouchEvent>

class KCL_EXPORT TouchDetector :
#ifdef KCL_QTQUICK2
    public QQuickItem
#else
    public QDeclarativeItem
#endif
{
    Q_OBJECT
    Q_PROPERTY(bool touchDeviceActive READ touchDeviceActive NOTIFY touchDeviceActiveChanged)
public:
#ifdef KCL_QTQUICK2
    explicit TouchDetector(QQuickItem *parent = 0);
#else
    explicit TouchDetector(QDeclarativeItem *parent = 0);
#endif
    virtual ~TouchDetector();

    bool touchDeviceActive() const;

signals:
    void touchDeviceActiveChanged();

protected:
#ifdef KCL_QTQUICK2
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
#endif

private:
    bool touchDeviceActive_;

    void setTouchDeviceActive(bool value);
};

#endif // TOUCHDETECTOR_H
