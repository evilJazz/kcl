#include "KCL/sceneutils.h"
#include "KCL/debug.h"

#ifdef KCL_QTQUICK2
    #include <QQuickWindow>
    #define zValue() z()
#else
    #include <QGraphicsScene>
#endif

SceneUtils::SceneUtils(QObject *parent) :
    QObject(parent)
{
}

static bool itemZOrder_sort(KCLGraphicsItem *lhs, KCLGraphicsItem *rhs)
{
    return lhs->zValue() < rhs->zValue();
}

struct PositionPaintOrderFilter : PaintOrderFilter
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

QList<KCLGraphicsItem *> SceneUtils::paintOrderChildItems(KCLGraphicsItem *item, bool recursive, PaintOrderFilter *filter)
{
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

#ifdef KCL_QTQUICK2

QVariantList SceneUtils::getAllItemsInScene(QQuickItem *item, qreal sceneX, qreal sceneY)
{
    QVariantList result;

    PositionPaintOrderFilter filter;
    filter.scenePoint = item->mapToScene(QPointF(sceneX, sceneY));

    QList<QQuickItem *> itemList = SceneUtils::paintOrderChildItems(item->window()->contentItem(), true, &filter);

    for (int i = 0; i < itemList.count(); ++i)
    {
        QObject *item = itemList.at(i);
        if (item)
            result.append(qVariantFromValue(item));
    }

    return result;
}

QVariantList SceneUtils::getItemsBelow(QQuickItem *item, qreal sceneX, qreal sceneY)
{
    QVariantList result;

    PositionPaintOrderFilter filter;
    filter.scenePoint = item->mapToScene(QPointF(sceneX, sceneY));

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

#else

QVariantList SceneUtils::getAllItemsInScene(QDeclarativeItem *item, qreal sceneX, qreal sceneY)
{
    QPointF scenePos = item->mapToScene(QPointF(sceneX, sceneY));

    QVariantList result;
    QList<QGraphicsItem *> itemList = item->scene()->items(scenePos);

    for (int i = 0; i < itemList.count(); ++i)
    {
        QObject *item = qgraphicsitem_cast<QDeclarativeItem *>(itemList.at(i));
        if (item)
            result.append(qVariantFromValue(item));
    }

    return result;
}

QVariantList SceneUtils::getItemsBelow(QDeclarativeItem *item, qreal sceneX, qreal sceneY)
{
    QPointF scenePos = item->mapToScene(QPointF(sceneX, sceneY));

    QVariantList result;
    QList<QGraphicsItem *> itemList = item->scene()->items(scenePos);

    int index = itemList.indexOf(item);

    if (index > -1)
    {
        for (int i = index + 1; i < itemList.count(); ++i)
        {
            QObject *item = qgraphicsitem_cast<QDeclarativeItem *>(itemList.at(i));
            if (item)
                result.append(qVariantFromValue(item));
        }
    }

    return result;
}
#endif
