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

#ifndef CURSORAREA_H
#define CURSORAREA_H

#include "KCL/kcl_global.h"

#ifdef KCL_QTQUICK2
    #include <QQuickItem>
    #include <QQmlComponent>
#else
    #include <QDeclarativeItem>
#endif

class KCL_EXPORT CursorArea :
#ifdef KCL_QTQUICK2
    public QQuickItem
#else
    public QDeclarativeItem
#endif
{
    Q_OBJECT
    Q_ENUMS(Cursor)
    Q_PROPERTY(Cursor cursor READ cursor WRITE setCursor NOTIFY cursorChanged)
#ifdef KCL_QTQUICK2
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
#else
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
#endif
    Q_PROPERTY(int hotX READ hotX WRITE setHotX NOTIFY hotXChanged)
    Q_PROPERTY(int hotY READ hotY WRITE setHotY NOTIFY hotYChanged)
public:
#ifdef KCL_QTQUICK2
    explicit CursorArea(QQuickItem *parent = 0);
#else
    explicit CursorArea(QDeclarativeItem *parent = 0);
#endif
    virtual ~CursorArea();

    enum Cursor {
        DefaultCursor,
        ArrowCursor,
        SizeHorCursor,
        SizeVerCursor,
        SizeAllCursor,
        SplitHCursor,
        SplitVCursor,
        WaitCursor,
        PointingHandCursor
    };

    Cursor cursor() const { return cursor_; }
    void setCursor(Cursor cursor);

    Q_INVOKABLE void setOnDesktop();
    Q_INVOKABLE void restoreOnDesktop();

#ifdef KCL_QTQUICK2
    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);
#else
    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *delegate);
#endif

    int hotX() const { return hotX_; }
    int hotY() const { return hotY_; }

    void setHotX(int newValue);
    void setHotY(int newValue);

signals:
    void cursorChanged();
    void delegateChanged();
    void hotXChanged();
    void hotYChanged();

private:
    Cursor cursor_;

#ifdef KCL_QTQUICK2
    QQmlComponent *delegate_;
#else
    QDeclarativeComponent *delegate_;
#endif
    int hotX_;
    int hotY_;

    QCursor createCustomCursor();
};

#endif // CURSORAREA_H
