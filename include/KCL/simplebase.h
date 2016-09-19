/***************************************************************************
 *   Copyright (C) 2016 Andre Beckedorf                                    *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifndef SIMPLEBASE_H
#define SIMPLEBASE_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>

class SimpleBase : public QObject
{
    Q_OBJECT
public:
    explicit SimpleBase(QObject *parent = 0);
    virtual ~SimpleBase();

    Q_INVOKABLE QVariant load(const QString &key);
    Q_INVOKABLE bool save(const QString &key, const QVariant &value);

    static void setGlobalDatabaseFilename(const QString fileName);
    static QString globalDatabaseFilename();

private:
    QSqlDatabase db_;

    QVariant load(const QString &key, QString column);

    bool loadDatabase();
    void createDatabase();
    void checkDatabaseError(const QSqlQuery &result, const QByteArray &functionName);
};

#endif // SIMPLEBASE_H
