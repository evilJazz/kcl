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

#include "KCL/simplebase.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>
#include <QDebug>

#include "KCL/filesystemutils.h"

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

static QString *globalDatabaseFilename = NULL;
static QMutex globalDbMutex_(QMutex::Recursive);
static QSettings::Format simpleBaseFormat_ = QSettings::InvalidFormat;

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

static bool qSettingsWriteSimpleBaseFunc(QIODevice &device, const QSettings::SettingsMap &map)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QFileDevice *file = qobject_cast<QFileDevice *>(&device);
#else
    QFile *file = qobject_cast<QFileDevice *>(&device);
#endif

    if (file && !file->fileName().isEmpty())
    {
        if (!device.isOpen())
            device.open(QIODevice::ReadWrite);

        device.seek(0);
        device.write("V1");

        QString dbFilename = file->fileName() + ".sqlite";

        SimpleBase sb;
        sb.setDatabaseConnection(SimpleBase::UseSQLiteDatabase);
        sb.setDatabaseFilename(dbFilename);
        sb.beginTransaction();

        foreach (const QString &key, map.keys())
            sb.save(key, map.value(key));

        sb.endTransaction();

        return true;
    }

    qDebug("qSettingsReadSimpleBaseFunc: input device is not QFile, can't serialize QSettings to SimpleBase.");
    return false;
}

static bool qSettingsReadSimpleBaseFunc(QIODevice &device, QSettings::SettingsMap &map)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QFileDevice *file = qobject_cast<QFileDevice *>(&device);
#else
    QFile *file = qobject_cast<QFileDevice *>(&device);
#endif

    if (file && !file->fileName().isEmpty())
    {
        if (!device.isOpen())
            device.open(QIODevice::ReadOnly);

        QByteArray header = device.read(2);
        device.close();

        if (header != "V1")
            return false;

        QString dbFilename = file->fileName() + ".sqlite";

        SimpleBase sb;
        sb.setDatabaseConnection(SimpleBase::UseSQLiteDatabase);
        sb.setDatabaseFilename(dbFilename);

        foreach (const QString &key, sb.keys())
            map.insert(key, sb.load(key));

        return true;
    }

    qDebug("qSettingsReadSimpleBaseFunc: input device is not QFile, can't serialize QSettings to SimpleBase.");
    return false;
}

QSettings::Format SimpleBase::simpleBaseFormat()
{
    if (simpleBaseFormat_ == QSettings::InvalidFormat)
        simpleBaseFormat_ = QSettings::registerFormat("simplebase", &qSettingsReadSimpleBaseFunc, &qSettingsWriteSimpleBaseFunc);

    return simpleBaseFormat_;
}

void SimpleBase::startingUpdate()
{
    if (loadDatabase())
    {
        if (db_.driver()->hasFeature(QSqlDriver::Transactions))
            db_.transaction();
    }
}

void SimpleBase::finishUpdate()
{
    if (db_.driver()->hasFeature(QSqlDriver::Transactions))
        db_.commit();
}

SimpleBase::SimpleBase(QObject *parent) :
    QObject(parent),
    databaseConnection_(UseExistingDefaultDatabaseOrSQLiteDatabaseAsFallback),
    sanitizedTableName_("kv_store")
{
    DGUARDMETHODTIMED;
    databaseFilename_ = globalDatabaseFilename();
}

SimpleBase::~SimpleBase()
{
    DGUARDMETHODTIMED;
}

QVariant SimpleBase::load(const QString &key)
{
    return load(key, QString::null);
}

QStringList SimpleBase::keys()
{
    QStringList result;

    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        q.prepare(QString("SELECT `key` FROM `%1`;").arg(sanitizedTableName_));

        q.exec();

        while (q.next())
            result << q.value(0).toString();
    }

    return result;
}

bool SimpleBase::exists(const QString &key)
{
    DGUARDMETHODTIMED;
    QMutexLocker l(&globalDbMutex_);

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        q.prepare(QString("SELECT COUNT(*) FROM `%1` WHERE `key` = ?;").arg(sanitizedTableName_));
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

        q.prepare(QString("SELECT `%1` FROM `%2` WHERE `key` = ?;").arg(column).arg(sanitizedTableName_));
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

        bool result = false;
        int run = 0;

        do
        {
            ++run;

            q.prepare(QString("REPLACE INTO `%1`(`key`, `value`, `date_created`, `date_modified`) VALUES(?, ?, " + date_created_preset + ", CURRENT_TIMESTAMP);").arg(sanitizedTableName_));
            q.bindValue(0, key);
            q.bindValue(1, value);

            if (!created.isNull())
                q.bindValue(2, created);

            result = q.exec();

            // Did this execution fail? Perhaps we need to create the table and try again?
            if (!result && db_.driverName() == "QSQLITE")
                createTable();
        }
        while (!result && run == 1);

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

        q.prepare(QString("DELETE FROM `%1` WHERE `key` = ?;").arg(sanitizedTableName_));
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

        q.prepare(QString("REPLACE INTO `%1`(`key`, `value`, `date_created`, `date_modified`) SELECT ?, value, date_created, date_modified FROM `kv_store` WHERE `key` = ?;").arg(sanitizedTableName_));
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

void SimpleBase::setDatabaseConnection(SimpleBase::DatabaseConnection databaseConnection)
{
    if (db_.isOpen())
        qWarning("Can't change the database connection when the database is open.");
    else
    {
        if (databaseConnection != databaseConnection_)
        {
            databaseConnection_ = databaseConnection;
            emit databaseConnectionChanged();
        }
    }
}

SimpleBase::DatabaseConnection SimpleBase::databaseConnection() const
{
    return databaseConnection_;
}

void SimpleBase::setDatabaseFilename(const QString fileName)
{
    if (db_.isOpen())
        qWarning("Can't change the database filename when the database is open.");
    else
    {
        if (fileName != databaseFilename_)
        {
            databaseFilename_ = fileName;
            emit databaseFilenameChanged();
        }
    }
}

QString SimpleBase::databaseFilename() const
{
    return databaseFilename_;
}

void SimpleBase::setTableName(const QString tableName)
{
    QString newTableName = sanitizeDbString(tableName);

    if (newTableName != sanitizedTableName_)
    {
        sanitizedTableName_ = newTableName;
        emit tableNameChanged();
    }
}

QString SimpleBase::tableName() const
{
    return sanitizedTableName_;
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
        if (databaseConnection_ == UseExistingDefaultDatabaseOrSQLiteDatabaseAsFallback ||
            databaseConnection_ == UseExistingDefaultDatabase)
        {
            // Use default database connection by default. This can be any database connector supported by Qt.
            db_ = QSqlDatabase::database();
        }

        // Not connected or connection error? Try a reconnect...
        if (db_.isValid() && (!db_.isOpen() || db_.lastError().type() != QSqlError::NoError))
            db_.open();

        if (!db_.isOpen())
        {
            // Still no luck? Try with our internal SQLite database instead...
            if (databaseConnection_ == UseExistingDefaultDatabaseOrSQLiteDatabaseAsFallback ||
                databaseConnection_ == UseSQLiteDatabase)
            {
                if (databaseFilename().isEmpty())
                {
                    qWarning("No database filename set for SimpleBase. This will not work.");
                    return false;
                }

                db_ = QSqlDatabase::database(databaseFilename());
                if (!db_.isValid())
                {
                    if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
                    {
                        qWarning("No QSQLITE database driver available.");
                        return false;
                    }

                    bool dataBasePresent = QFileInfo::exists(databaseFilename());
                    db_ = QSqlDatabase::addDatabase("QSQLITE", databaseFilename());
                    db_.setDatabaseName(databaseFilename());
                    db_.open();
                    if (!db_.isOpen())
                    {
                        qWarning("Could not open the database %s.", databaseFilename().toUtf8().constData());
                        return false;
                    }

                    if (!dataBasePresent)
                        createTable();
                }
            }
            else
            {
                qWarning("Could not connect to the existing default database.");
                return false;
            }
        }
    }

    return true;
}

bool SimpleBase::closeDatabase()
{
    if (db_.isOpen())
        db_.close();
}

void SimpleBase::beginTransaction()
{
    beginUpdate();
}

void SimpleBase::endTransaction()
{
    endUpdate();
}

bool SimpleBase::createTable()
{
    DGUARDMETHODTIMED;
    QSqlQuery result;

    result = db_.exec(
        QString(
            "CREATE TABLE IF NOT EXISTS `%1`"
            "("
            "    `key` TEXT PRIMARY KEY NOT NULL,"
            "    `value` TEXT,"
            "    `date_created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "    `date_modified` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
            ");"
        ).arg(sanitizedTableName_)
    );

    checkDatabaseError(result, "createDatabase");

    return !result.lastError().isValid();
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

QString SimpleBase::sanitizeDbString(const QString &s)
{
    QString output = s;
    output = output.remove(QRegExp("[^a-zA-Z\\d\\s]")).simplified().replace(" ", "_");
    return output;
}
