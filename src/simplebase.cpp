/****************************************************************************
**
** Copyright (C) 2016 Andre Beckedorf
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

#include "KCL/simplebase.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

static QString *globalDatabaseFilename = NULL;
static QMutex globalDbMutex_(QMutex::Recursive);

QString getGlobalDatabaseFilenameSingleton()
{
    if (!globalDatabaseFilename)
        globalDatabaseFilename = new QString();

    return *globalDatabaseFilename;
}

void setGlobalDatabaseFilenameSingleton(const QString value)
{
    if (!globalDatabaseFilename)
        globalDatabaseFilename = new QString();

    *globalDatabaseFilename = value;
}

SimpleBase::SimpleBase(QObject *parent) :
    QObject(parent)
{
    DGUARDMETHODTIMED;
}

SimpleBase::~SimpleBase()
{
    DGUARDMETHODTIMED;
}

QVariant SimpleBase::load(const QString &key)
{
    return load(key, QString::null);
}

bool SimpleBase::exists(const QString &key)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        q.prepare("SELECT COUNT(*) FROM `kv_store` WHERE `key` = ?;");
        q.bindValue(0, key);

        q.exec();

        if (!q.next())
        {
            checkDatabaseError(q, "exists");
            return false;
        }

        return q.value(0).toInt() > 0;
    }
    else
        return false;
}

QVariant SimpleBase::load(const QString &key, QString column)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        if (column.isEmpty())
            column = "value";

        q.prepare(QString("SELECT `%1` FROM `kv_store` WHERE `key` = ?;").arg(column));
        q.bindValue(0, key);

        q.exec();

        if (!q.next())
        {
            checkDatabaseError(q, "load");
            return QVariant();
        }

        return q.value(0);
    }
    else
        return QVariant();
}

bool SimpleBase::save(const QString &key, const QVariant &value)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        QVariant created = load(key, "date_created");
        QString date_created_preset = "?";

        if (created.isNull())
            date_created_preset = "CURRENT_TIMESTAMP";

        q.prepare("REPLACE INTO `kv_store`(`key`, `value`, `date_created`, `date_modified`) VALUES(?, ?, " + date_created_preset + ", CURRENT_TIMESTAMP);");
        q.bindValue(0, key);
        q.bindValue(1, value);

        if (!created.isNull())
            q.bindValue(2, created);

        bool result = q.exec();

        if (!result)
            checkDatabaseError(q, "save");

        return result;
    }

    return false;
}

bool SimpleBase::remove(const QString &key)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        q.prepare("DELETE FROM `kv_store` WHERE `key` = ?;");
        q.bindValue(0, key);

        bool result = q.exec();

        if (!result)
            checkDatabaseError(q, "remove");

        result = result && q.numRowsAffected() > 0;
        return result;
    }
    else
        return false;
}

bool SimpleBase::copy(const QString &srcKey, const QString &dstKey)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        q.prepare("REPLACE INTO `kv_store`(`key`, `value`, `date_created`, `date_modified`) SELECT ?, value, date_created, date_modified FROM `kv_store` WHERE `key` = ?;");
        q.bindValue(0, dstKey);
        q.bindValue(1, srcKey);

        bool result = q.exec();

        if (!result)
            checkDatabaseError(q, "copy");

        result = result && q.numRowsAffected() > 0;
        return result;
    }

    return false;
}

bool SimpleBase::rename(const QString &existingKey, const QString &newKey)
{
    DGUARDMETHODTIMED;
    // TODO: Improve by updating key on server...

    bool result = copy(existingKey, newKey);

    if (result)
        result = remove(existingKey);

    return result;
}

void SimpleBase::setGlobalDatabaseFilename(const QString fileName)
{
    if (fileName != getGlobalDatabaseFilenameSingleton())
    {
        setGlobalDatabaseFilenameSingleton(fileName);
    }
}

QString SimpleBase::globalDatabaseFilename()
{
    return getGlobalDatabaseFilenameSingleton();
}

bool SimpleBase::loadDatabase()
{
    DGUARDMETHODTIMED;

    if (!db_.isValid() || !db_.isOpen() || db_.isOpenError() || db_.lastError().type() != QSqlError::NoError)
    {
        // Use default database connection by default. This can be any database connector supported by Qt.
        db_ = QSqlDatabase::database();

        // Not connected or connection error? Try a reconnect...
        if (db_.isValid() && (!db_.isOpen() || db_.lastError().type() != QSqlError::NoError))
            db_.open();

        // Still no luck? Try with our internal SQLite database instead...
        if (!db_.isOpen())
        {
            if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
            {
                qWarning("No QSQLITE database driver available.");
                return false;
            }

            if (globalDatabaseFilename().isEmpty())
            {
                qWarning("No database filename set for SimpleBase. This will not work.");
                return false;
            }

            bool dataBasePresent = QFileInfo::exists(globalDatabaseFilename());

            db_ = QSqlDatabase::addDatabase("QSQLITE");
            db_.setDatabaseName(globalDatabaseFilename());
            db_.open();
            if (!db_.isOpen())
            {
                qWarning("Could not open database %s.", globalDatabaseFilename().toUtf8().constData());
                return false;
            }

            if (!dataBasePresent)
                createDatabase();
        }
    }

    return true;
}

void SimpleBase::createDatabase()
{
    DGUARDMETHODTIMED;
    QSqlQuery result;

    result = db_.exec(
        "CREATE TABLE IF NOT EXISTS `kv_store`"
        "("
        "    `key` TEXT PRIMARY KEY NOT NULL,"
        "    `value` TEXT,"
        "    `date_created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "    `date_modified` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");"
    );

    checkDatabaseError(result, "createDatabase");
}

void SimpleBase::checkDatabaseError(const QSqlQuery &result, const QByteArray &functionName)
{
    QSqlError lastError = result.lastError();

    if (lastError.isValid())
    {
        qWarning() << "SQL error in function " << functionName << ": " << lastError.text() << '\n' << flush;
        db_.open(); // Make sure we reconnect to the database on error!
    }
}
