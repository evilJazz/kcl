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
