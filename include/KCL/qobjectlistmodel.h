#ifndef QOBJECTLISTMODEL_H
#define QOBJECTLISTMODEL_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QAbstractListModel>

class KCL_EXPORT QObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY changed)
public:
    enum Roles {
        ItemRole = Qt::UserRole + 1
    };

    explicit QObjectListModel(const QString roleName = QString("item"), QObject *parent = NULL) : QAbstractListModel(parent)
    {
        roleNames_[ItemRole] = roleName.toLatin1();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        setRoleNames(roleNames_);
#endif
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
    void forceFullUpdate()
    {
        beginResetModel();
        endResetModel();
        emit changed();
    }

    void setItems(const QList<QObject *> &items)
    {
        beginResetModel();
        items_.clear();
        items_ = items;
        endResetModel();

        emit changed();
    }

    void clear()
    {
        beginResetModel();
        items_.clear();
        endResetModel();

        emit changed();
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual QHash<int,QByteArray> roleNames() const { return roleNames_; }
#endif

signals:
    void changed();

protected:
    QHash<int, QByteArray> roleNames_;
    QList<QObject *> items_;
};

// Providing these conversion methods as kludges because C++ templates of QObject
// derived classes that contain Q_OBJECT macro are not possible.

template <typename T> const QList<QObject *> &castToObjectList(const QList<T> &list)
{
    return reinterpret_cast<const QList<QObject *> &>(list);
}

template <typename T> QList<QObject *> &castToObjectList(QList<T> &list)
{
    return reinterpret_cast<QList<QObject *> &>(list);
}

class KCL_EXPORT QSharedObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY changed)
public:
    enum Roles {
        ItemRole = Qt::UserRole + 1
    };

    explicit QSharedObjectListModel(const QString roleName = QString("item"), QObject *parent = NULL) : QAbstractListModel(parent)
    {
        roleNames_[ItemRole] = roleName.toLatin1();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        setRoleNames(roleNames_);
#endif
    }

    virtual ~QSharedObjectListModel()
    {
        clear();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const { return items_.count(); }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        const int row = index.row();
        if (row < 0 || row >= items_.size() || role != ItemRole)
            return QVariant();

        return qVariantFromValue(static_cast<QObject *>(items_.at(index.row()).data()));
    }

    Q_INVOKABLE QObject *get(int index) const { return index < 0 || index >= items_.count() ? NULL : items_.at(index).data(); }

    QModelIndex indexOfItem(QObject * item) const
    {
        int itemIndex = items_.indexOf(QSharedPointer<QObject>(item));
        if (itemIndex > -1)
            return index(itemIndex);
        else
            return QModelIndex();
    }

    const QList< QSharedPointer<QObject> > &items() const { return items_; }
    void forceFullUpdate()
    {
        beginResetModel();
        endResetModel();
        emit changed();
    }

    void setItems(const QList< QSharedPointer<QObject> > &items)
    {
        beginResetModel();
        items_.clear();
        items_ = items;
        endResetModel();

        emit changed();
    }

    void clear()
    {
        beginResetModel();
        items_.clear();
        endResetModel();

        emit changed();
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual QHash<int, QByteArray> roleNames() const { return roleNames_; }
#endif

signals:
    void changed();

protected:
    QHash<int, QByteArray> roleNames_;
    QList< QSharedPointer<QObject> > items_;
};

// Providing these conversion methods as kludges because C++ templates of QObject
// derived classes that contain Q_OBJECT macro are not possible.

template <typename T> const QList< QSharedPointer<QObject> > &castToSharedObjectList(const QList< QSharedPointer<T> > &list)
{
    return reinterpret_cast<const QList< QSharedPointer<QObject> > &>(list);
}

template <typename T> QList< QSharedPointer<QObject> > &castToSharedObjectList(QList< QSharedPointer<T> > &list)
{
    return reinterpret_cast<QList< QSharedPointer<QObject> > &>(list);
}

#endif // QOBJECTLISTMODEL_H
