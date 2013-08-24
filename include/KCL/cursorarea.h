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

#ifdef KCL_QTQUICK2
    #include <QQuickItem>
#else
    #include <QDeclarativeItem>
#endif

class CursorArea :
#ifdef KCL_QTQUICK2
    public QQuickItem
#else
    public QDeclarativeItem
#endif
{
    Q_OBJECT
    Q_ENUMS(Cursor)
    Q_PROPERTY(Cursor cursor READ cursor WRITE setCursor NOTIFY cursorChanged)
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

signals:
    void cursorChanged();

private:
    Cursor cursor_;
};

#endif // CURSORAREA_H
