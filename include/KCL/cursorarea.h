/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
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

#ifndef CURSORAREA_H
#define CURSORAREA_H

#include "KCL/kcl_global.h"

#ifdef KCL_QTQUICK2
    #include <QQuickItem>
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
    Q_PROPERTY(QDeclarativeComponent* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
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

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent* delegate);

    int hotX() const { return hotX_; }
    int hotY() const { return hotY_; }

    int setHotX(int newValue);
    int setHotY(int newValue);

signals:
    void cursorChanged();
    void delegateChanged();
    void hotXChanged();
    void hotYChanged();

private:
    Cursor cursor_;

    QDeclarativeComponent *delegate_;
    int hotX_;
    int hotY_;

    QCursor createCustomCursor();
};

#endif // CURSORAREA_H
