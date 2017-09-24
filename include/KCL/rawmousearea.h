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

#ifndef RAWMOUSEAREA_H
#define RAWMOUSEAREA_H

#include "KCL/kcl_global.h"

#include <QDeclarativeItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

class KCL_EXPORT DeclarativeMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)
public:
    DeclarativeMouseEvent(int x, int y, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
        : x_(x), y_(y), button_(button), buttons_(buttons), modifiers_(modifiers), accepted_(true) {}

    int x() const { return x_; }
    int y() const { return y_; }
    int button() const { return button_; }
    int buttons() const { return buttons_; }
    int modifiers() const { return modifiers_; }

    bool isAccepted() { return accepted_; }
    void setAccepted(bool accepted) { accepted_ = accepted; }

private:
    int x_;
    int y_;
    Qt::MouseButton button_;
    Qt::MouseButtons buttons_;
    Qt::KeyboardModifiers modifiers_;
    bool accepted_;
};

class KCL_EXPORT RawMouseArea : public QDeclarativeItem
{
    Q_OBJECT
public:
    RawMouseArea(QDeclarativeItem *parent = 0);
    virtual ~RawMouseArea();

    Q_INVOKABLE void grabMouse();
    Q_INVOKABLE void ungrabMouse();

signals:
    void pressed(DeclarativeMouseEvent *mouse);
    void moved(DeclarativeMouseEvent *mouse);
    void released(DeclarativeMouseEvent *mouse);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // RAWMOUSEAREA_H
