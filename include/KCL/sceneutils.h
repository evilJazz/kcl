#ifndef SCENEUTILS_H
#define SCENEUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariantList>

#ifdef KCL_QTQUICK2
    #include <QQuickItem>
#else
    #include <QDeclarativeItem>
#endif

class SceneUtils : public QObject
{
    Q_OBJECT
public:
    explicit SceneUtils(QObject *parent = 0);

#ifdef QTQUICK2
    Q_INVOKABLE static QVariantList getAllItemsInScene(QQuickItem *item, qreal sceneX, qreal sceneY);
    Q_INVOKABLE static QVariantList getItemsBelow(QQuickItem *item, qreal sceneX, qreal sceneY);
#else
    Q_INVOKABLE static QVariantList getAllItemsInScene(QDeclarativeItem *item, qreal sceneX, qreal sceneY);
    Q_INVOKABLE static QVariantList getItemsBelow(QDeclarativeItem *item, qreal sceneX, qreal sceneY);
#endif
};

#endif // SCENEUTILS_H