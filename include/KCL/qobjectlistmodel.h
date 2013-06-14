#ifndef QOBJECTLISTMODEL_H
#define QOBJECTLISTMODEL_H

#include <QObject>
#include <QList>
#include <QAbstractListModel>

#include <QDeclarativeListProperty>

class QObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY changed)
public:
    enum Roles {
        ItemRole = Qt::UserRole + 1
    };

    explicit QObjectListModel(const QString roleName = QString("item"), QObject *parent = NULL) : QAbstractListModel(parent)
    {
        QHash<int, QByteArray> names;
        names[ItemRole] = roleName.toLatin1();
        setRoleNames(names);
    }

    virtual ~QObjectListModel()
    {
        clear();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const { return items_.count(); }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        const int row = index.row();
        if (row < 0 || row >= items_.size() || role != ItemRole)
            return QVariant();

        return qVariantFromValue(static_cast<QObject *>(items_.at(index.row())));
    }

    Q_INVOKABLE QObject *get(int index) const { return index < 0 || index >= items_.count() ? NULL : items_.at(index); }

    QModelIndex indexOfItem(QObject * item) const
    {
        int itemIndex = items_.indexOf(item);
        if (itemIndex > -1)
            return index(itemIndex);
        else
            return QModelIndex();
    }

    const QList<QObject *> &items() const { return items_; }
    void forceFullUpdate() { reset(); emit changed(); }

    void setItems(const QList<QObject *> &items)
    {
        beginInsertRows(QModelIndex(), 0, items.size() - 1);
        items_ = items;
        endInsertRows();
        emit changed();
    }

    void clear()
    {
        beginRemoveRows(QModelIndex(), 0, items_.size() - 1);
        items_.clear();
        endRemoveRows();
        emit changed();
    }

signals:
    void changed();

protected:
    QList<QObject *> items_;
};

// Providing these conversion methods as kludges because C++ templates of QObject
// derived classes that consist Q_OBJECT macro are not possible.

template <typename T> const QList<QObject *> &castToObjectList(const QList<T> &list)
{
    return reinterpret_cast<const QList<QObject *> &>(list);
}

template <typename T> QList<QObject *> &castToObjectList(QList<T> &list)
{
    return reinterpret_cast<QList<QObject *> &>(list);
}

#endif // QOBJECTLISTMODEL_H
