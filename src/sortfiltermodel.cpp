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

#include "KCL/sortfiltermodel.h"

SortFilterModel::SortFilterModel(QObject* parent) :
    QSortFilterProxyModel(parent),
    startIndex_(0),
    endIndex_(-1)
{
}

SortFilterModel::~SortFilterModel()
{
}

QHash<int, QByteArray> SortFilterModel::sourceModelRoleNames() const
{
    QHash<int, QByteArray> result;

    if (sourceModel())
        result = sourceModel()->roleNames();
    else
        result = QSortFilterProxyModel::roleNames();

    return result;
}

void SortFilterModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    if (sourceModel)
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        setRoleNames(sourceModelRoleNames());
#endif
    }
}

QVariantMap SortFilterModel::get(int index) const
{
    QVariantMap result;

    QHash<int, QByteArray> roleNames = this->roleNames();

    QModelIndex modelIndex = this->index(index, 0);

    foreach (int role, roleNames.keys())
    {
        QVariant value = data(modelIndex, role);
        result.insert(roleNames.value(role), value);
    }

    return result;
}

void SortFilterModel::setStartIndex(int startIndex)
{
    if (startIndex != startIndex_)
    {
        beginResetModel();
        startIndex_ = qMax(0, startIndex);
        endResetModel();
        emit startIndexChanged();
        emit countChanged();
    }
}

void SortFilterModel::setEndIndex(int endIndex)
{
    if (endIndex != endIndex_)
    {
        beginResetModel();
        endIndex_ = endIndex >= startIndex_ ? endIndex : -1;
        endResetModel();
        emit endIndexChanged();
        emit countChanged();
    }
}

bool SortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (source_row >= startIndex_ && (endIndex_ == -1 || source_row <= endIndex_))
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    else
        return false;
}
