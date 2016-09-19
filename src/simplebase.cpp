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

QVariant SimpleBase::load(const QString &key, QString column)
{
    DGUARDMETHODTIMED;

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        if (column.isEmpty())
            column = "value";

        q.prepare(QString("SELECT `%1` FROM kv_store WHERE key = ?;").arg(column));
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

    if (loadDatabase())
    {
        QSqlQuery q(db_);

        QVariant created = load(key, "date_created");

        q.prepare("REPLACE INTO kv_store(key, value, date_created, date_modified) VALUES(?, ?, ?, CURRENT_TIMESTAMP);");
        q.bindValue(0, key);
        q.bindValue(1, value);
        q.bindValue(2, created);

        bool result = q.exec();

        if (!result)
            checkDatabaseError(q, "save");

        return result;
    }

    return false;
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

    if (!db_.isOpen())
    {
        db_ = QSqlDatabase::database();

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
        "CREATE TABLE IF NOT EXISTS kv_store"
        "("
        "    key TEXT PRIMARY KEY NOT NULL,"
        "    value TEXT,"
        "    date_created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "    date_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ")"
    );

    checkDatabaseError(result, "createDatabase");
}

void SimpleBase::checkDatabaseError(const QSqlQuery &result, const QByteArray &functionName)
{
    QSqlError lastError = result.lastError();

    if (lastError.isValid())
        qWarning() << "SQL error in function " << functionName << ": " << lastError.text() << '\n' << flush;
}