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

    Q_INVOKABLE static QVariantList getAllItemsInScene(KCLDeclarativeItem *item, qreal sceneX, qreal sceneY);
    Q_INVOKABLE static QVariantList getItemsBelow(KCLDeclarativeItem *item, qreal sceneX, qreal sceneY);
};

#endif // SCENEUTILS_H
