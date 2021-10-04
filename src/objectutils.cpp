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

#include "KCL/objectutils.h"

#include "KCL/debug.h"

#include <QMetaType>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>

ObjectUtils::ObjectUtils(QObject *parent) :
    QObject(parent)
{
}

QString ObjectUtils::className(QObject *target)
{
    return target ? QString::fromLatin1(target->metaObject()->className()) : QString::null;
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
    return target ? target->inherits(className.toLatin1()) : false;
}

QObject *ObjectUtils::objectify(const QVariant &object)
{
    return static_cast<QObject *>(object.canConvert<QObject *>() ? object.value<QObject *>() : NULL);
}

QVariant ObjectUtils::objectToVariant(QObject *object)
{
    return QVariant::fromValue(object);
}

QString ObjectUtils::objectAsString(QObject *object)
{
    return object ? QString().sprintf("%s(%s%p)", object->metaObject()->className(), (!object->objectName().isEmpty() ? object->objectName() + " " : "").toLatin1().constData(), object) : QString("NULL");
}

void ObjectUtils::dumpObjectTree(QObject *target)
{
    ObjectUtils::dumpObjectTreeRecursive(0, target);
}

void ObjectUtils::dumpParentTree(QObject *target)
{
    int level = 0;
    QByteArray buf;

    while (target)
    {
        buf.fill(' ', level * 4);

        DPRINTF("%s%s", buf.constData(), objectAsString(target).toLatin1().constData());
        target = target->parent();
        ++level;
    }
}

QStringList ObjectUtils::toStringList(const QList<QByteArray> &list)
{
    QStringList strings;

    foreach (const QByteArray &item, list)
        strings.append(QString::fromLocal8Bit(item));

    return strings;
}

void ObjectUtils::insertMetaMethod(QVariantMap &info, const QMetaMethod &method)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QString signature = QString::fromLatin1(method.methodSignature());
    QString name = QString::fromLatin1(method.name());
#else
    QString signature = QString::fromLatin1(method.signature());
    int paramStartIndex = signature.indexOf('(');
    QString name = paramStartIndex > -1 ? signature.left(paramStartIndex + 1) : signature;
#endif

    QString typeName = QString::fromLatin1(method.typeName());

    info.insert("name", name);
    info.insert("typeName", typeName);
    info.insert("signature", signature);

    QStringList paramNames = toStringList(method.parameterNames());
    QStringList paramTypes = toStringList(method.parameterTypes());

    info.insert("parameterNames", paramNames);
    info.insert("parameterTypes", paramTypes);

    QStringList params;
    int paramCount = paramNames.count();

    for (int i = 0; i < paramCount; ++i)
        params << paramTypes.at(i) + " " + paramNames.at(i);

    QString fullSignature = QString("%1(%2)")
        .arg(name)
        .arg(params.join(", "));

    info.insert("fullSignature",
        (method.methodType() != QMetaMethod::Signal ?
            typeName + " " : "") + fullSignature);
}

QVariantList ObjectUtils::getAllMethods(QObject *object)
{
    if (!object) return QVariantList();
    return getAllMethods(object->metaObject());
}

QVariantList ObjectUtils::getAllMethods(const QMetaObject *metaObject)
{
    QVariantList methods;

    for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
    {
        QMetaMethod method = metaObject->method(i);

        if (method.methodType() != QMetaMethod::Signal &&
            method.access() == QMetaMethod::Public)
        {
            QVariantMap info;
            insertMetaMethod(info, method);
            methods << info;
        }
    }

    return methods;
}

QVariantList ObjectUtils::getAllSignals(QObject *object)
{
    if (!object) return QVariantList();
    return getAllSignals(object->metaObject());
}

QVariantList ObjectUtils::getAllSignals(const QMetaObject *metaObject)
{
    QVariantList methods;

    for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
    {
        QMetaMethod method = metaObject->method(i);

        if (method.methodType() == QMetaMethod::Signal &&
            method.access() == QMetaMethod::Public)
        {
            QVariantMap info;
            insertMetaMethod(info, method);
            methods << info;
        }
    }

    return methods;
}

QVariantList ObjectUtils::getAllProperties(QObject *object)
{
    if (!object) return QVariantList();
    return getAllProperties(object->metaObject());
}

QVariantList ObjectUtils::getAllProperties(const QMetaObject *metaObject)
{
    QVariantList properties;

    for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject->property(i);

        QVariantMap info;

        QString name = QString::fromLatin1(prop.name());
        QString typeName = QString::fromLatin1(prop.typeName());

        info.insert("name", name);
        info.insert("typeName", typeName);

        QString signature = QString("%1 %2")
            .arg(typeName)
            .arg(name);

        QString fullSignature = QString("%1property %2")
            .arg(!prop.isWritable() ? "readonly " : "")
            .arg(signature);

        info.insert("signature", signature);
        info.insert("fullSignature", fullSignature);

        properties << info;
    }

    return properties;
}

QVariantMap ObjectUtils::introspectClass(const QString &className)
{
    QVariantMap result;

    int typeId = QMetaType::type(className.toUtf8());

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (typeId != QMetaType::UnknownType)
#else
    if (typeId != 0)
#endif
    {
        result.insert("typeName", QMetaType::typeName(typeId));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        const QMetaObject *metaObj = QMetaType::metaObjectForType(typeId);
        result = introspectMetaObject(metaObj);
#else
        result.insert("error", "Not implemented yet for Qt 4.");
        result.insert("hasError", true);
#endif
    }
    else
    {
        result.insert("error", "Unknown type");
        result.insert("hasError", true);
    }

    return result;
}

QVariantMap ObjectUtils::introspectObject(QObject *object)
{
    return introspectMetaObject(object ? object->metaObject() : NULL);
}

QVariantMap ObjectUtils::introspectMetaObject(const QMetaObject *metaObj)
{
    QVariantMap result;

    if (metaObj)
    {
        result.insert("typeName", metaObj->className());
        result.insert("className", metaObj->className());
        result.insert("properties", getAllProperties(metaObj));
        result.insert("methods", getAllMethods(metaObj));
        result.insert("signals", getAllSignals(metaObj));
    }
    else
    {
        result.insert("error", "Unknown type");
        result.insert("hasError", true);
    }

    return result;
}

void ObjectUtils::dumpObjectTreeRecursive(int level, QObject *object)
{
    if (object)
    {
        QByteArray buf;
        buf.fill(' ', level * 4);

        if (level == 0)
        {
            DPRINTF("%s%s parent: %s", buf.constData(), objectAsString(object).toLatin1().constData(), objectAsString(object->parent()).toLatin1().constData());
        }
        else
        {
            DPRINTF("%s%s", buf.constData(), objectAsString(object).toLatin1().constData());
        }

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

QObject *ObjectUtils::findObjectByObjectName(QObject *rootElement, const QString &objectName)
{
    if (rootElement)
    {
        if (rootElement->objectName() == objectName)
            return rootElement;

        const QObjectList list = rootElement->children();
        for (QObjectList::const_iterator it = list.begin(); it != list.end(); it++)
        {
            QObject *object = findObjectByObjectName((*it), objectName);
            if (object)
                return object;
        }
    }

    return NULL;
}
