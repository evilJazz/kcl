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

#include "KCL/cursorarea.h"

#ifdef KCL_QTQUICK2
#include <QCursor>
#define QDeclarativeItem QQuickItem
#endif

CursorArea::CursorArea(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    cursor_(DefaultCursor),
    delegate_(NULL),
    hotX_(0),
    hotY_(0)
{
}

CursorArea::~CursorArea()
{
}

void CursorArea::setCursor(Cursor cursor)
{
    if (cursor_ != cursor)
    {
        switch (cursor) {
        case DefaultCursor:
            QDeclarativeItem::setCursor(QCursor());
            break;
        case ArrowCursor:
            QDeclarativeItem::setCursor(Qt::ArrowCursor);
            break;
        case SizeHorCursor:
            QDeclarativeItem::setCursor(Qt::SizeHorCursor);
            break;
        case SizeVerCursor:
            QDeclarativeItem::setCursor(Qt::SizeVerCursor);
            break;
        case SizeAllCursor:
            QDeclarativeItem::setCursor(Qt::SizeAllCursor);
            break;
        case SplitHCursor:
            QDeclarativeItem::setCursor(Qt::SplitHCursor);
            break;
        case SplitVCursor:
            QDeclarativeItem::setCursor(Qt::SplitVCursor);
            break;
        case WaitCursor:
            QDeclarativeItem::setCursor(Qt::WaitCursor);
            break;
        case PointingHandCursor:
            QDeclarativeItem::setCursor(Qt::PointingHandCursor);
            break;
        default:
            return;
        }

        cursor_ = cursor;
        emit cursorChanged();
    }
}

#include <QGuiApplication>
#include <QCursor>
#include <QGraphicsScene>
#include <QPainter>

QCursor CursorArea::createCustomCursor()
{
    QCursor result;

    if (delegate_)
    {
        QDeclarativeItem* item = qobject_cast<QDeclarativeItem *>(delegate_->create());

        QGraphicsScene scene;
        scene.addItem(item);

        QPixmap pixmap(scene.sceneRect().width(), scene.sceneRect().height());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        scene.render(&painter);

        result = QCursor(pixmap, hotX_, hotY_);
    }
    else
        result = QDeclarativeItem::cursor();

    return result;
}

void CursorArea::setOnDesktop()
{
    QCursor cursor = createCustomCursor();
    QGuiApplication::setOverrideCursor(cursor);
}

void CursorArea::restoreOnDesktop()
{
    QGuiApplication::restoreOverrideCursor();
}

QDeclarativeComponent *CursorArea::delegate() const
{
    return delegate_;
}

void CursorArea::setDelegate(QDeclarativeComponent *delegate)
{
    if (delegate_ != delegate)
    {
        delegate_ = delegate;
        emit delegateChanged();
    }
}

int CursorArea::setHotX(int newValue)
{
    if (hotX_ != newValue)
    {
        hotX_ = newValue;
        emit hotXChanged();
    }
}

int CursorArea::setHotY(int newValue)
{
    if (hotY_ != newValue)
    {
        hotY_ = newValue;
        emit hotYChanged();
    }
}
