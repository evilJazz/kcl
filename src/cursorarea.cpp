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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include <QCursor>
#include <QPainter>

#ifdef KCL_QTQUICK2

#include <QQuickWindow>

QCursor CursorArea::createCustomCursor()
{
    QCursor result;

    if (delegate_)
    {
        QQuickWindow *window = new QQuickWindow();
        QQuickItem *item = qobject_cast<QQuickItem *>(delegate_->create());

        window->resize(item->width(), item->height());
        item->setParent(window);
        item->setParentItem(window->contentItem());

        qDebug() << window->width() << window->height();

        QImage image = window->grabWindow();
        QPixmap pixmap = QPixmap::fromImage(image);

        result = QCursor(pixmap, hotX_, hotY_);

        delete window;
    }
    else
        result = QQuickItem::cursor();

    return result;
}

void CursorArea::setOnDesktop()
{
    qWarning("CursorArea::setOnDesktop is not implemented for KCL_QTQUICK2.");
}

void CursorArea::restoreOnDesktop()
{
    qWarning("CursorArea::restoreOnDesktop is not implemented for KCL_QTQUICK2.");
}

QQmlComponent *CursorArea::delegate() const
{
    return delegate_;
}

void CursorArea::setDelegate(QQmlComponent *delegate)
{
    if (delegate_ != delegate)
    {
        delegate_ = delegate;
        emit delegateChanged();
    }
}

#else

#include <QGraphicsScene>

QCursor CursorArea::createCustomCursor()
{
    QCursor result;

    if (delegate_)
    {
        QDeclarativeItem* item = qobject_cast<QDeclarativeItem *>(delegate_->create());

        QGraphicsScene scene;
        scene.addItem(item); // takes ownership

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QGuiApplication::setOverrideCursor(cursor);
#else
    QApplication::setOverrideCursor(cursor);
#endif
}

void CursorArea::restoreOnDesktop()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QGuiApplication::restoreOverrideCursor();
#else
    QApplication::restoreOverrideCursor();
#endif
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
#endif

void CursorArea::setHotX(int newValue)
{
    if (hotX_ != newValue)
    {
        hotX_ = newValue;
        emit hotXChanged();
    }
}

void CursorArea::setHotY(int newValue)
{
    if (hotY_ != newValue)
    {
        hotY_ = newValue;
        emit hotYChanged();
    }
}
