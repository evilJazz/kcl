#ifndef OBJECTUTILS_H
#define OBJECTUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>

class KCL_EXPORT ObjectUtils : public QObject
{
    Q_OBJECT
public:
    explicit ObjectUtils(QObject *parent = 0);

    Q_INVOKABLE static bool isNull(QObject *target);
    Q_INVOKABLE static QString className(QObject *target);

    Q_INVOKABLE static bool sameClass(QObject *object1, QObject *object2);
    Q_INVOKABLE static bool sameReference(QObject *object1, QObject *object2);
    Q_INVOKABLE static bool inherits(QObject *target, const QString &className);

    Q_INVOKABLE static QObject *objectify(const QVariant &object);

    Q_INVOKABLE static QString objectAsString(QObject *object);
    Q_INVOKABLE static void dumpObjectTree(QObject *target);

private:
    static void dumpObjectTreeRecursive(int level, QObject *object);
};

#endif // OBJECTUTILS_H
