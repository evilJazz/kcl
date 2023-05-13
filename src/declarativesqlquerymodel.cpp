/****************************************************************************
**
** Copyright (C) 2011-2021 Andre Beckedorf
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

#include "KCL/declarativesqlquerymodel.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlIndex>
#include <QModelIndex>
#include <QCryptographicHash>
#include <QDebug>

#ifdef KCL_QTQUICK2
    #define QDeclarativeEngine QQmlEngine
    #define QDeclarativeContext QQmlContext
#else
    #include <QDeclarativeEngine>
    #include <QDeclarativeContext>
#endif

//#define debugMsg(x, ...) qDebug(x)

#if !defined(debugMsg)
#define debugMsg(x, ...)
#endif

QSqlDatabase getDatabaseInstance()
{
    if (!QSqlDatabase::contains("sqlite"))
        return QSqlDatabase::addDatabase("QSQLITE", "sqlite");
    else
        return QSqlDatabase::database("sqlite");
}

DeclarativeSqlQueryModel::DeclarativeSqlQueryModel(QObject *parent) :
    QSqlTableModel(parent, getDatabaseInstance()),
    loading_(false),
    operationMode_(FreeFormQueryAndReadOnly),
    sortOrder_(Qt::AscendingOrder),
    sortColumn_(-1)
{
    setEditStrategy(OnFieldChange);

    database_ = QSqlTableModel::database();
    connect(this, SIGNAL(modelReset()), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SIGNAL(countChanged()));
}

DeclarativeSqlQueryModel::~DeclarativeSqlQueryModel()
{
}

QString DeclarativeSqlQueryModel::offlineStorageDatabaseFilename(const QString &dbName)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(dbName.toUtf8());
    QByteArray md5 = hash.result().toHex();

    QDeclarativeContext *context = QDeclarativeEngine::contextForObject(this);
    QString offlineStoragePath = context->engine()->offlineStoragePath();

    return offlineStoragePath + "/Databases/" + md5 + ".sqlite";
}

void DeclarativeSqlQueryModel::setQuery(const QString &query)
{
    if (query != query_)
    {
        query_ = query;
        setOperationMode(FreeFormQueryAndReadOnly);
        updateContent();
    }
}

QString DeclarativeSqlQueryModel::lastError()
{
    return QSqlQueryModel::lastError().text();
}

void DeclarativeSqlQueryModel::refresh()
{
    updateContent();
}

void DeclarativeSqlQueryModel::generateRoleNames()
{
    QHash<int, QByteArray> oldRoleNames = roleNames();
    QHash<int, QByteArray> roleNames;

    QSqlRecord rec = record();

    for( int i = 0; i < rec.count(); ++i)
        roleNames[Qt::UserRole + i + 1] = rec.fieldName(i).toUtf8();

    if (roleNames != oldRoleNames)
    {
        setRoleNames(roleNames);
        emit roleNamesChanged();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void DeclarativeSqlQueryModel::setRoleNames(QHash<int, QByteArray> roleNames)
{
    roleNames_ = roleNames;
}
#endif

QVariant DeclarativeSqlQueryModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    if (role < Qt::UserRole)
        value = QSqlQueryModel::data(index, role);
    else
    {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }

    return value;
}

void DeclarativeSqlQueryModel::setFileName(const QString &fileName)
{
    if (fileName != fileName_)
    {
        fileName_ = fileName;
        updateContent();
        emit fileNameChanged();
    }
}

void DeclarativeSqlQueryModel::updateContent()
{
    if (!loading_ && !fileName_.isEmpty())
    {
        if (database_.databaseName() != fileName_)
            database_.setDatabaseName(fileName_);

        QString oldLastError = lastError();
        QString oldPrimaryKey = primaryKey();

        if (!database_.open())
            qWarning("Could not open database %s", fileName_.toUtf8().constData());

        if (!table_.isEmpty())
        {
            debugMsg("Setting table: %s, filter: %s", table_.toUtf8().constData(), filter_.toUtf8().constData());
            if (!oldLastError.isEmpty() || QSqlTableModel::tableName() != table_) QSqlTableModel::setTable(table_);
            if (!oldLastError.isEmpty() || QSqlTableModel::filter() != filter_) QSqlTableModel::setFilter(filter_);
            QSqlTableModel::setSort(sortColumn_, sortOrder_);
            QSqlTableModel::select();
        }
        else if (!query_.isEmpty())
        {
            debugMsg("Setting query: %s", query_.toUtf8().constData());

            QSqlTableModel::setTable("");
            QSqlTableModel::setFilter("");
            QSqlTableModel::setSort(0, sortOrder());
            QSqlQueryModel::setQuery(query_, database_);
        }

        if (lastError() != oldLastError)
            emit lastErrorChanged();

        if (primaryKey() != oldPrimaryKey)
            emit primaryKeyChanged();

        debugMsg("Current model -> table: %s, filter: %s, selectStatement: %s, orderByClause: %s,  query: %s",
            QSqlTableModel::tableName().toUtf8().constData(),
            QSqlTableModel::filter().toUtf8().constData(),
            QSqlTableModel::selectStatement().toUtf8().constData(),
            QSqlTableModel::orderByClause().toUtf8().constData(),
            QSqlTableModel::query().lastQuery().toUtf8().constData()
        );
    }
}

void DeclarativeSqlQueryModel::setOperationMode(DeclarativeSqlQueryModel::OperationMode newMode)
{
    if (newMode != operationMode_)
    {
        operationMode_ = newMode;

        if (operationMode_ == FreeFormQueryAndReadOnly)
        {
            filter_ = QString::null;
            table_ = QString::null;
            sortColumn_ = -1;
            sortOrder_ = Qt::AscendingOrder;
        }
        else
            query_ = QString::null;

        emit operationModeChanged();
    }
}

void DeclarativeSqlQueryModel::setFilter(const QString &filter)
{
    if (filter != filter_)
    {
        filter_ = filter;
        setOperationMode(FilteredAndModifiable);
        updateContent();
    }
}

void DeclarativeSqlQueryModel::setTable(const QString &tableName)
{
    if (tableName != table_)
    {
        table_ = tableName;
        setOperationMode(FilteredAndModifiable);
        updateContent();
    }
}

void DeclarativeSqlQueryModel::setSortOrder(Qt::SortOrder order)
{
    if (order != sortOrder_)
    {
        sortOrder_ = order;
        emit sortingChanged();
        updateContent();
    }
}

void DeclarativeSqlQueryModel::setSortColumn(int column)
{
    if (column != sortColumn_)
    {
        sortColumn_ = column;
        emit sortingChanged();
        updateContent();
    }
}

QString DeclarativeSqlQueryModel::primaryKey() const
{
    QSqlIndex result = QSqlTableModel::primaryKey();
    return result.isEmpty() ? "" : result.name();
}

void DeclarativeSqlQueryModel::queryChange()
{
    generateRoleNames();
    emit queryChanged();
}

void DeclarativeSqlQueryModel::classBegin()
{
    loading_ = true;
}

void DeclarativeSqlQueryModel::componentComplete()
{
    loading_ = false;
    updateContent();
}

bool DeclarativeSqlQueryModel::requireWriteModeSuccessful() const
{
    if (operationMode_ == DeclarativeSqlQueryModel::FreeFormQueryAndReadOnly)
    {
        qWarning("Cannot use this operation in read only mode.");
        return false;
    }
    else
        return true;
}

bool DeclarativeSqlQueryModel::append(const QVariantMap &dict)
{
    return insert(-1, dict);
}

void DeclarativeSqlQueryModel::clear()
{
    //if (!requireWriteModeSuccessful()) return;
    QSqlTableModel::clear();
}

QVariant DeclarativeSqlQueryModel::get(int index) const
{
    QVariantMap row;

    QSqlRecord rec = record(index);

    for( int i = 0; i < rec.count(); ++i)
        row.insert(rec.fieldName(i), rec.value(i));

    return row;
}

bool DeclarativeSqlQueryModel::insert(int index, const QVariantMap &dict)
{
    if (!requireWriteModeSuccessful()) return false;

    QSqlRecord rec = record();

    foreach (QString key, dict.keys())
        rec.setValue(key, dict.value(key));

    bool result = insertRecord(index, rec);
    debugMsg("insert: index: %d, result: %d", index, result);
    return result;
}

bool DeclarativeSqlQueryModel::move(int from, int to)
{
    qWarning("DeclarativeSqlQueryModel::move is NOT IMPLEMENTED.");
    return false;
}

bool DeclarativeSqlQueryModel::remove(int index)
{
    if (!requireWriteModeSuccessful()) return false;
    return removeRow(index);
}

bool DeclarativeSqlQueryModel::set(int index, const QVariantMap &dict)
{
    if (!requireWriteModeSuccessful() || dict.isEmpty()) return false;
    if (index < 0) return false;

    QSqlRecord rec = record(index);

    foreach (QString key, dict.keys())
        rec.setValue(key, dict.value(key));

    return setRecord(index, rec);
}

bool DeclarativeSqlQueryModel::setProperty(int index, const QString &property, const QVariant &value)
{
    if (!requireWriteModeSuccessful()) return false;
    if (index < 0) return false;

    debugMsg("setProperty: index: %d, property: %s, value: %s", index, property.toUtf8().constData(), value.toString().toUtf8().constData());

    QSqlRecord rec = record(index);
    rec.setValue(property, value);
    bool result = setRecord(index, rec);
    debugMsg("result: %d", result);

    return result;
}
