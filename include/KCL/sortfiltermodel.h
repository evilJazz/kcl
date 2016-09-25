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

#ifndef SORTFILTERMODEL_H
#define SORTFILTERMODEL_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSize>
#include <QSortFilterProxyModel>

class KCL_EXPORT SortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex NOTIFY startIndexChanged)
    Q_PROPERTY(int endIndex READ endIndex WRITE setEndIndex NOTIFY endIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    SortFilterModel(QObject* parent = 0);
    virtual ~SortFilterModel();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual QHash<int, QByteArray> roleNames() const { return sourceModelRoleNames(); }
#endif

    int count() const { return rowCount(); }
    Q_INVOKABLE QVariantMap get(int index) const;

    virtual void setSourceModel(QAbstractItemModel *sourceModel);

    int startIndex() const { return startIndex_; }
    int endIndex() const { return endIndex_; }

    void setStartIndex(int startIndex);
    void setEndIndex(int endIndex);

signals:
    void startIndexChanged();
    void endIndexChanged();
    void countChanged();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    int startIndex_;
    int endIndex_;

    QHash<int, QByteArray> sourceModelRoleNames() const;
};

#endif // SORTFILTERMODEL_H
