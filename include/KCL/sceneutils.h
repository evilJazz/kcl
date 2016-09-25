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

#ifndef SCENEUTILS_H
#define SCENEUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariantList>

#ifdef KCL_QTQUICK2
    #include <QQuickItem>

    #define KCLGraphicsItem QQuickItem
    #define KCLDeclarativeItem QQuickItem
#else
    #include <QGraphicsItem>
    #include <QDeclarativeItem>

    #define KCLGraphicsItem QGraphicsItem
    #define KCLDeclarativeItem QDeclarativeItem
#endif

struct KCL_EXPORT PaintOrderFilter
{
    virtual bool matches(KCLGraphicsItem *item) { return false; }
    virtual bool filterChildren(KCLGraphicsItem *item) { return false; }
};

class KCL_EXPORT SceneUtils : public QObject
{
    Q_OBJECT
public:
    explicit SceneUtils(QObject *parent = 0);

    static QList<KCLGraphicsItem *> paintOrderChildItems(KCLGraphicsItem *item, bool recursive, PaintOrderFilter *filter = NULL);
    static QList<KCLGraphicsItem *> getItemsAtPositionInItem(KCLGraphicsItem *item, qreal itemX, qreal itemY);

// Can't use the KCLDeclarativeItem macro here because moc does not understand it...
#ifdef KCL_QTQUICK2
    Q_INVOKABLE static QVariantList getAllItemsInScene(QQuickItem *item, qreal itemX, qreal itemY);
    Q_INVOKABLE static QVariantList getItemsBelow(QQuickItem *item, qreal itemX, qreal itemY);

    Q_INVOKABLE static void takeItemFromScene(QQuickItem *item);
    Q_INVOKABLE static void reparentItem(QQuickItem *item, QQuickItem *newParent);

    Q_INVOKABLE static QVariant mappedBoundingRect(QQuickItem *targetItem, QQuickItem *sourceItem, const QRect &rect = QRect());

    Q_INVOKABLE static QRectF sceneItemsBoundingRect(QQuickItem *sceneItem);
    Q_INVOKABLE static QVariant takeImageFromScene(QQuickItem *sceneItem);

    Q_INVOKABLE static void forceUpdate(QQuickItem *item);

    Q_INVOKABLE static void grabMouse(QQuickItem *item);
    Q_INVOKABLE static void ungrabMouse(QQuickItem *item);
#else
    Q_INVOKABLE static QVariantList getAllItemsInScene(QDeclarativeItem *item, qreal itemX, qreal itemY);
    Q_INVOKABLE static QVariantList getItemsBelow(QDeclarativeItem *item, qreal itemX, qreal itemY);

    Q_INVOKABLE static void takeItemFromScene(QDeclarativeItem *item);
    Q_INVOKABLE static void reparentItem(QDeclarativeItem *item, QDeclarativeItem *newParent);

    Q_INVOKABLE static QVariant mappedBoundingRect(QGraphicsObject *targetItem, QGraphicsObject *sourceItem, const QRect &rect = QRect());

    Q_INVOKABLE static QRectF sceneItemsBoundingRect(QGraphicsObject *sceneItem);
    Q_INVOKABLE static QVariant takeImageFromScene(QGraphicsObject *sceneItem);

    Q_INVOKABLE static void forceUpdate(QGraphicsObject *item);

    Q_INVOKABLE static void grabMouse(QGraphicsObject *item);
    Q_INVOKABLE static void ungrabMouse(QGraphicsObject *item);
#endif
};

#endif // SCENEUTILS_H
