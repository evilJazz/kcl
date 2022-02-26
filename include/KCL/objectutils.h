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

#ifndef OBJECTUTILS_H
#define OBJECTUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QVariantList>

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
    Q_INVOKABLE static QVariant objectToVariant(QObject *object);

    Q_INVOKABLE static QString objectAsString(QObject *object);
    Q_INVOKABLE static void dumpObjectTree(QObject *target);
    Q_INVOKABLE static void dumpParentTree(QObject *target);

    Q_INVOKABLE static QVariantList getAllMethods(QObject *object, bool recurseSuperClasses = false, bool namesOnly = false);
    static QVariantList getAllMethods(const QMetaObject *metaObject, bool namesOnly = false);

    Q_INVOKABLE static QVariantList getAllSignals(QObject *object, bool recurseSuperClasses = false, bool namesOnly = false);
    static QVariantList getAllSignals(const QMetaObject *metaObject, bool namesOnly = false);

    Q_INVOKABLE static QVariantList getAllProperties(QObject *object, bool recurseSuperClasses = false, bool namesOnly = false);
    static QVariantList getAllProperties(const QMetaObject *metaObject, bool namesOnly = false);

    Q_INVOKABLE static QVariantMap introspectClass(const QString &className);
    Q_INVOKABLE static QVariantMap introspectObject(QObject *object);
    Q_INVOKABLE static QVariantMap introspectMetaObject(const QMetaObject *metaObj);

    Q_INVOKABLE static QObject *findObjectByObjectName(QObject *rootElement, const QString &objectName);

private:
    static void dumpObjectTreeRecursive(int level, QObject *object);
    static QStringList toStringList(const QList<QByteArray> &list);
    static void insertMetaMethod(QVariantMap &info, const QMetaMethod &method, bool namesOnly);
};

#endif // OBJECTUTILS_H
