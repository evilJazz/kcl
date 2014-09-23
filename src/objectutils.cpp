#include "KCL/objectutils.h"

#include "KCL/debug.h"

ObjectUtils::ObjectUtils(QObject *parent) :
    QObject(parent)
{
}

QString ObjectUtils::className(QObject *target)
{
    return QString::fromLatin1(target->metaObject()->className());
}

bool ObjectUtils::sameClass(QObject *object1, QObject *object2)
{
    return className(object1) == className(object2);
}

bool ObjectUtils::sameReference(QObject *object1, QObject *object2)
{
    return object1 == object2;
}

bool ObjectUtils::inherits(QObject *target, const QString &className)
{
    return target->inherits(className.toLatin1());
}

QObject *ObjectUtils::objectify(const QVariant &object)
{
    return static_cast<QObject *>(object.canConvert<QObject *>() ? object.value<QObject *>() : NULL);
}

QString ObjectUtils::objectAsString(QObject *object)
{
    return object ? QString().sprintf("%s(%s%p)", object->metaObject()->className(), (!object->objectName().isEmpty() ? object->objectName() + " " : "").toLatin1().constData(), object) : QString("NULL");
}

void ObjectUtils::dumpObjectTree(QObject *target)
{
    ObjectUtils::dumpObjectTreeRecursive(0, target);
}

void ObjectUtils::dumpObjectTreeRecursive(int level, QObject *object)
{
    if (object)
    {
        QByteArray buf;
        buf.fill(' ', level * 4);

        if (level == 0)
            DPRINTF("%s%s parent: %s", buf.constData(), objectAsString(object).toLatin1().constData(), objectAsString(object->parent()).toLatin1().constData());
        else
            DPRINTF("%s%s", buf.constData(), objectAsString(object).toLatin1().constData());

        QObjectList children = object->children();
        if (!children.isEmpty())
        {
            for (int i = 0; i < children.size(); ++i)
                dumpObjectTreeRecursive(level + 1, children.at(i));
        }
    }
    else if (level == 0)
    {
        DPRINTF("%s", objectAsString(object).toLatin1().constData());
    }
}

bool ObjectUtils::isNull(QObject *target)
{
    return target == NULL;
}
