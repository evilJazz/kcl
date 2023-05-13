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

#ifndef DECLARATIVESQLQUERYMODEL_H
#define DECLARATIVESQLQUERYMODEL_H

#ifdef KCL_QTQUICK2
    #include <QtQuick>
#else
    #include <QDeclarativeListProperty>
    #include <QDeclarativeParserStatus>
#endif

#include <QSqlTableModel>

class DeclarativeSqlQueryModel :
    public QSqlTableModel,
#ifdef KCL_QTQUICK2
    public QQmlParserStatus
#else
    public QDeclarativeParserStatus
#endif
{
    Q_OBJECT

    Q_ENUMS(OperationMode)
    Q_ENUMS(EditStrategy)

    Q_PROPERTY(OperationMode operationMode READ operationMode NOTIFY operationModeChanged)
    Q_PROPERTY(EditStrategy editStrategy READ editStrategy WRITE setEditStrategy)

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)

    // Properties for FreeFormQueryAndReadOnly OperationMode
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

    // Properties for FilteredAndModifiable OperationMode
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY queryChanged)
    Q_PROPERTY(QString table READ tableName WRITE setTable NOTIFY queryChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortingChanged)
    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY sortingChanged)

    Q_PROPERTY(QString primaryKey READ primaryKey NOTIFY primaryKeyChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
    enum OperationMode {
        FreeFormQueryAndReadOnly,
        FilteredAndModifiable
    };

    enum EditStrategy {
        OnFieldChange,
        OnRowChange,
        OnManualSubmit
    };

    DeclarativeSqlQueryModel(QObject *parent = 0);
    virtual ~DeclarativeSqlQueryModel();

    OperationMode operationMode() const { return operationMode_; }

    void setEditStrategy(EditStrategy strategy) { QSqlTableModel::setEditStrategy(static_cast<QSqlTableModel::EditStrategy>(strategy)); }
    EditStrategy editStrategy() const { return static_cast<EditStrategy>(QSqlTableModel::editStrategy()); }

    QString fileName() const { return fileName_; }
    void setFileName(const QString &fileName);

    // Getter/Setter for FreeFormQueryAndReadOnly OperationMode
    QString query() const { return query_; }
    void setQuery(const QString &query);

    // Getter/Setter for FilteredAndModifiable OperationMode
    virtual void setFilter(const QString &filter);
    virtual void setTable(const QString &tableName);

    void setSortOrder(Qt::SortOrder order);
    Qt::SortOrder sortOrder() const { return sortOrder_; }

    void setSortColumn(int column);
    int sortColumn() const { return sortColumn_; }

    QString primaryKey() const;

    QString lastError();
    Q_INVOKABLE void refresh();

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE QString offlineStorageDatabaseFilename(const QString &dbName);
    Q_INVOKABLE int fieldIndex(const QString &fieldName) const { return QSqlTableModel::fieldIndex(fieldName); }

    // ListModel like interface:
    Q_INVOKABLE bool append(const QVariantMap &dict);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariant get(int index) const;
    Q_INVOKABLE int count() const { return rowCount(); }
    Q_INVOKABLE bool insert(int index, const QVariantMap &dict);
    Q_INVOKABLE bool move(int from, int to);
    Q_INVOKABLE bool remove(int index);
    Q_INVOKABLE bool set(int index, const QVariantMap &dict);
    Q_INVOKABLE bool setProperty(int index, const QString &property, const QVariant &value);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual QHash<int, QByteArray> roleNames() const { return roleNames_; }
#endif

signals:
    void fileNameChanged();
    void queryChanged();
    void lastErrorChanged();
    void countChanged();
    void operationModeChanged();
    void sortingChanged();
    void primaryKeyChanged();
    void roleNamesChanged();

protected:
    void queryChange();

    // QDeclarativeParserStatus interface
    void classBegin();
    void componentComplete();

private:
    bool loading_;
    QString fileName_;
    QSqlDatabase database_;
    OperationMode operationMode_;

    QString filter_;
    QString table_;
    Qt::SortOrder sortOrder_;
    int sortColumn_;

    QString query_;

    void updateContent();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QHash<int, QByteArray> roleNames_;
    void setRoleNames(QHash<int, QByteArray> roleNames);
#endif

    void generateRoleNames();

    void setOperationMode(DeclarativeSqlQueryModel::OperationMode newMode);

    bool requireWriteModeSuccessful() const;
};

#endif // DECLARATIVESQLQUERYMODEL_H
