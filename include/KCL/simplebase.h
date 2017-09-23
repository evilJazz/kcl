/****************************************************************************
**
** Copyright (C) 2011-2017 Andre Beckedorf
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
#include "KCL/updatelocker.h"

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QSettings>
#include <QMutex>

class SimpleBase : public QObject, private UpdateLocker
{
    Q_OBJECT
    Q_ENUMS(DatabaseConnection)
    Q_PROPERTY(QString databaseFilename READ databaseFilename WRITE setDatabaseFilename NOTIFY databaseFilenameChanged)
    Q_PROPERTY(DatabaseConnection databaseConnection READ databaseConnection WRITE setDatabaseConnection NOTIFY databaseConnectionChanged)
    Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
public:
    explicit SimpleBase(QObject *parent = 0);
    virtual ~SimpleBase();

    enum DatabaseConnection
    {
        UseExistingDefaultDatabaseOrSQLiteDatabaseAsFallback = 0,
        UseExistingDefaultDatabase = 1,
        UseSQLiteDatabase = 2
    };

    void setDatabaseConnection(DatabaseConnection databaseConnection);
    DatabaseConnection databaseConnection() const;

    void setDatabaseFilename(const QString fileName);
    QString databaseFilename() const;

    void setTableName(const QString tableName);
    QString tableName() const;

    Q_INVOKABLE bool loadDatabase();
    Q_INVOKABLE bool closeDatabase();

    Q_INVOKABLE void beginTransaction();
    Q_INVOKABLE void endTransaction();

    /* Database CRUD-style operations */
    Q_INVOKABLE QStringList keys();
    Q_INVOKABLE bool exists(const QString &key);
    Q_INVOKABLE QVariant load(const QString &key);
    Q_INVOKABLE bool save(const QString &key, const QVariant &value, bool saveOnlyIfModified = false);
    Q_INVOKABLE bool remove(const QString &key);
    Q_INVOKABLE bool copy(const QString &srcKey, const QString &dstKey);
    Q_INVOKABLE bool rename(const QString &existingKey, const QString &newKey);

    /* Static helpers */
    static void setGlobalDatabaseFilename(const QString fileName);
    static QString globalDatabaseFilename();

    static QSettings::Format simpleBaseFormat();

signals:
     void databaseFilenameChanged();
     void databaseConnectionChanged();
     void tableNameChanged();

protected:
    virtual void startingUpdate();
    virtual void finishUpdate();

private:
    QSqlDatabase db_;
    DatabaseConnection databaseConnection_;
    QString databaseFilename_;
    QString sanitizedTableName_;

    QSqlQuery executeLoadQuery(const QString &key, QString column);

    bool createTable();
    void checkDatabaseError(const QSqlQuery &result, const QByteArray &functionName);
    QString sanitizeDbString(const QString &s);

    static QVariant stringToVariant(const QString &s);
    static QString variantToString(const QVariant &v);
    static QStringList splitArgs(const QString &s, int idx);
};

#endif // SIMPLEBASE_H
