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

#include "KCL/sceneutils.h"
#include "KCL/debug.h"

#ifdef KCL_QTQUICK2
    #include <QQuickWindow>
    #define zValue() z()
#else
    #include <QGraphicsScene>
    #include <QGraphicsView>
#endif

#include <QPainter>
#include <QImage>

#include <algorithm> // std::stable_sort

SceneUtils::SceneUtils(QObject *parent) :
    QObject(parent)
{
}

static bool itemZOrder_sort(KCLGraphicsItem *lhs, KCLGraphicsItem *rhs)
{
    return lhs->zValue() < rhs->zValue();
}

struct ScenePositionPaintOrderFilter : PaintOrderFilter
{
    QPointF scenePoint;

    virtual bool matches(KCLGraphicsItem *item)
    {
        QPointF p = item->mapFromScene(scenePoint);
        //qDebug("item: %p, className: %s, name: %s, contains(p): %s", item, item->metaObject()->className(), item->objectName().toUtf8().constData(), item->contains(p) ? "true" : "false");
        return item->contains(p);
    }

    virtual bool filterChildren(KCLGraphicsItem *item) { return true; }
};

struct ItemPositionPaintOrderFilter : PaintOrderFilter
{
    KCLGraphicsItem *parentItem;
    QPointF itemPoint;

    virtual bool matches(KCLGraphicsItem *item)
    {
        QPointF p = item->mapFromItem(parentItem, itemPoint);
        //qDebug("item: %p, className: %s, name: %s, contains(p): %s", item, item->metaObject()->className(), item->objectName().toUtf8().constData(), item->contains(p) ? "true" : "false");
        return item->contains(p);
    }

    virtual bool filterChildren(KCLGraphicsItem *item) { return true; }
};

QList<KCLGraphicsItem *> SceneUtils::paintOrderChildItems(KCLGraphicsItem *item, bool recursive, PaintOrderFilter *filter)
{
    if (!item)
        return QList<KCLGraphicsItem *>();

    QList<KCLGraphicsItem *> childItems = item->childItems();

    // If none of the items have set Z then the paint order list is the same as
    // the childItems list.  This is by far the most common case.
    bool haveZ = false;
    for (int i = 0; i < childItems.count(); ++i)
    {
        if (childItems.at(i)->zValue() != 0.)
        {
            haveZ = true;
            break;
        }
    }

    if (haveZ)
        std::stable_sort(childItems.begin(), childItems.end(), itemZOrder_sort);

    if (recursive)
    {
        QList<KCLGraphicsItem *> resultItems;
        for (int i = 0; i < childItems.count(); ++i)
        {
            KCLGraphicsItem *item = childItems.at(i);
            if (!filter || filter->matches(item))
                resultItems += item;

            if (!filter || filter->filterChildren(item))
                resultItems += paintOrderChildItems(item, true, filter);
        }

        return resultItems;
    }
    else
        return childItems;
}

QList<KCLGraphicsItem *> SceneUtils::getItemsAtPositionInItem(KCLGraphicsItem *item, qreal itemX, qreal itemY)
{
    ItemPositionPaintOrderFilter filter;
    filter.parentItem = item;
    filter.itemPoint = QPointF(itemX, itemY);
    return SceneUtils::paintOrderChildItems(item, true, &filter);
}

#ifdef KCL_QTQUICK2

QVariantList SceneUtils::getAllItemsInScene(QQuickItem *item, qreal itemX, qreal itemY)
{
    QVariantList result;

    if (!item || !item->window() || !item->window()->contentItem())
        return result;

    ScenePositionPaintOrderFilter filter;
    filter.scenePoint = item->mapToScene(QPointF(itemX, itemY));

    QList<QQuickItem *> itemList = SceneUtils::paintOrderChildItems(item->window()->contentItem(), true, &filter);

    for (int i = 0; i < itemList.count(); ++i)
    {
        QObject *item = itemList.at(i);
        if (item)
            result.append(qVariantFromValue(item));
    }

    return result;
}

QVariantList SceneUtils::getItemsBelow(QQuickItem *item, qreal itemX, qreal itemY)
{
    QVariantList result;

    if (!item)
        return result;

    ScenePositionPaintOrderFilter filter;
    filter.scenePoint = item->mapToScene(QPointF(itemX, itemY));

    QList<QQuickItem *> itemList = SceneUtils::paintOrderChildItems(item->window()->contentItem(), true, &filter);

    int index = itemList.indexOf(item);

    if (index > -1)
    {
        for (int i = 0; i < index; ++i)
        {
            QObject *item = itemList.at(i);
            if (item)
                result.append(qVariantFromValue(item));
        }
    }

    return result;
}

void SceneUtils::takeItemFromScene(QQuickItem *item)
{
    // We can't actually take the item from the scene
    // It will be moved to the parentless-item list of the associated QQuickWindow instead...
    if (item)
        reparentItem(item, NULL);
}

void SceneUtils::reparentItem(QQuickItem *item, QQuickItem *newParent)
{
    if (item)
        item->setParentItem(newParent);
}

QVariant SceneUtils::mappedBoundingRect(QQuickItem *targetItem, QQuickItem *sourceItem, const QRect &rect)
{
    if (!targetItem || !sourceItem)
        return QVariant();

    QRectF sourceRect = rect.isNull() ? sourceItem->boundingRect() : rect;
    return targetItem->mapRectFromItem(sourceItem, sourceRect);
}

QRectF SceneUtils::sceneItemsBoundingRect(QQuickItem *sceneItem)
{
    if (!sceneItem || !sceneItem->window() || !sceneItem->window()->contentItem())
        return QRectF();

    return sceneItem->window()->contentItem()->childrenRect();
}

QVariant SceneUtils::takeImageFromScene(QQuickItem *sceneItem)
{
    if (!sceneItem || !sceneItem->window())
        return QImage();

    return sceneItem->window()->grabWindow().convertToFormat(QImage::Format_ARGB32);
}

void SceneUtils::forceUpdate(QQuickItem *item)
{
    if (item)
        item->update();
}

void SceneUtils::grabMouse(QQuickItem *item)
{
    if (item)
        item->grabMouse();
}

void SceneUtils::ungrabMouse(QQuickItem *item)
{
    if (item)
        item->ungrabMouse();
}

#else

QVariantList SceneUtils::getAllItemsInScene(QDeclarativeItem *item, qreal itemX, qreal itemY)
{
    QVariantList result;

    if (!item || !item->scene())
        return result;

    QPointF scenePos = item->mapToScene(QPointF(itemX, itemY));
    QList<QGraphicsItem *> itemList = item->scene()->items(scenePos);

    for (int i = 0; i < itemList.count(); ++i)
    {
        QObject *item = qobject_cast<QDeclarativeItem *>(itemList.at(i));
        if (item)
            result.append(qVariantFromValue(item));
    }

    return result;
}

QVariantList SceneUtils::getItemsBelow(QDeclarativeItem *item, qreal itemX, qreal itemY)
{
    QVariantList result;

    if (!item || !item->scene())
        return result;

    QPointF scenePos = item->mapToScene(QPointF(itemX, itemY));
    QList<QGraphicsItem *> itemList = item->scene()->items(scenePos);

    int index = itemList.indexOf(item);

    if (index > -1)
    {
        for (int i = index + 1; i < itemList.count(); ++i)
        {
            QObject *item = qobject_cast<QDeclarativeItem *>(itemList.at(i));
            if (item)
                result.append(qVariantFromValue(item));
        }
    }

    return result;
}

void SceneUtils::takeItemFromScene(QDeclarativeItem *item)
{
    if (item && item->scene())
        item->scene()->removeItem(item);
    else
        reparentItem(item, NULL);
}

void SceneUtils::reparentItem(QDeclarativeItem *item, QDeclarativeItem *newParent)
{
    if (item)
        item->setParentItem(newParent);
}

QVariant SceneUtils::mappedBoundingRect(QGraphicsObject *targetItem, QGraphicsObject *sourceItem, const QRect &rect)
{
    QRectF sourceRect = rect.isNull() ? sourceItem->boundingRect() : rect;
    return targetItem->mapRectFromItem(sourceItem, sourceRect);
}

QRectF SceneUtils::sceneItemsBoundingRect(QGraphicsObject *sceneItem)
{
    if (!sceneItem || !sceneItem->scene())
        return QRectF();

    return sceneItem->scene()->itemsBoundingRect();
}

QVariant SceneUtils::takeImageFromScene(QGraphicsObject *sceneItem)
{
    if (!sceneItem || !sceneItem->scene())
        return QImage();

    QSize size;

    if (sceneItem->scene()->views().count() > 0)
        size = sceneItem->scene()->views().at(0)->size();
    else
        size = sceneItemsBoundingRect(sceneItem).size().toSize();

    QImage image(size, QImage::Format_ARGB32);
    if (!image.isNull())
    {
        QPainter p(&image);
        sceneItem->scene()->render(&p, QRectF(image.rect()), QRectF(QPointF(0, 0), QSizeF(size)));
    }

    return image;
}

void SceneUtils::forceUpdate(QGraphicsObject *item)
{
    item->update();
}

void SceneUtils::grabMouse(QGraphicsObject *item)
{
    item->grabMouse();
}

void SceneUtils::ungrabMouse(QGraphicsObject *item)
{
    item->ungrabMouse();
}

#endif
