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
    Q_INVOKABLE static QVariantList getAllItemsInScene(QQuickItem *item, qreal sceneX, qreal sceneY);
    Q_INVOKABLE static QVariantList getItemsBelow(QQuickItem *item, qreal sceneX, qreal sceneY);

    Q_INVOKABLE static void takeItemFromScene(QQuickItem *item);
    Q_INVOKABLE static void reparentItem(QQuickItem *item, QQuickItem *newParent);

    Q_INVOKABLE static QVariant mappedBoundingRect(QQuickItem *targetItem, QQuickItem *sourceItem, const QRect &rect = QRect());

    Q_INVOKABLE static QVariant takeImageFromScene(QQuickItem *sceneItem);
#else
    Q_INVOKABLE static QVariantList getAllItemsInScene(QDeclarativeItem *item, qreal sceneX, qreal sceneY);
    Q_INVOKABLE static QVariantList getItemsBelow(QDeclarativeItem *item, qreal sceneX, qreal sceneY);

    Q_INVOKABLE static void takeItemFromScene(QDeclarativeItem *item);
    Q_INVOKABLE static void reparentItem(QDeclarativeItem *item, QDeclarativeItem *newParent);

    Q_INVOKABLE static QVariant mappedBoundingRect(QDeclarativeItem *targetItem, QDeclarativeItem *sourceItem, const QRect &rect = QRect());

    Q_INVOKABLE static QVariant takeImageFromScene(QDeclarativeItem *sceneItem);
#endif
};

#endif // SCENEUTILS_H
