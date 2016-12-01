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

#ifndef SIMPLEBASE_H
#define SIMPLEBASE_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QMutex>

class SimpleBase : public QObject
{
    Q_OBJECT
public:
    explicit SimpleBase(QObject *parent = 0);
    virtual ~SimpleBase();

    Q_INVOKABLE QVariant load(const QString &key);
    Q_INVOKABLE bool exists(const QString &key);
    Q_INVOKABLE bool save(const QString &key, const QVariant &value);
    Q_INVOKABLE bool remove(const QString &key);
    Q_INVOKABLE bool copy(const QString &srcKey, const QString &dstKey);
    Q_INVOKABLE bool rename(const QString &existingKey, const QString &newKey);

    static void setGlobalDatabaseFilename(const QString fileName);
    static QString globalDatabaseFilename();

    bool loadDatabase();

private:
    QSqlDatabase db_;

    QVariant load(const QString &key, QString column);

    void createDatabase();
    void checkDatabaseError(const QSqlQuery &result, const QByteArray &functionName);
};

#endif // SIMPLEBASE_H
