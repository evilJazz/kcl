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
