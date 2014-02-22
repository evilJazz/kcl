#include "KCL/objectutils.h"

ObjectUtils::ObjectUtils(QObject *parent) :
    QObject(parent)
{
}

QByteArray ObjectUtils::className(QObject *target)
{
    return QByteArray(target->metaObject()->className());
}

bool ObjectUtils::sameClass(QObject *object1, QObject *object2)
{
    return className(object1) == className(object2);
}

bool ObjectUtils::sameReference(QObject *object1, QObject *object2)
{
    return object1 == object2;
}
