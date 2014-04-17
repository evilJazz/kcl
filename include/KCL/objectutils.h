#ifndef OBJECTUTILS_H
#define OBJECTUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>

class KCL_EXPORT ObjectUtils : public QObject
{
    Q_OBJECT
public:
    explicit ObjectUtils(QObject *parent = 0);

    Q_INVOKABLE static QString className(QObject *target);
    Q_INVOKABLE static bool sameClass(QObject *object1, QObject *object2);
    Q_INVOKABLE static bool sameReference(QObject *object1, QObject *object2);
    Q_INVOKABLE static bool inherits(QObject *target, const QString &className);

signals:

public slots:

};

#endif // OBJECTUTILS_H
