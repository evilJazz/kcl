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

    virtual ~QObjectListModel() {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const { return items_.count(); }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (index.row() < 0 || index.row() >= items_.size() || role != ItemRole)
            return QVariant();

        return qVariantFromValue(static_cast<QObject *>(items_.at(index.row())));
    }

    Q_INVOKABLE QObject *get(int index) const { return index < 0 || index >= items_.count() ? NULL : items_.at(index); }

    QModelIndex indexFromItem(QObject * item) const
    {
        int itemIndex = items_.indexOf(item);
        if (itemIndex > -1)
            return index(itemIndex);
        else
            return QModelIndex();
    }

    QList<QObject *> *items() { return &items_; }
    void forceFullUpdate() { reset(); emit changed(); }

    void clear()
    {
        qDeleteAll(items_);
        items_.clear();
        forceFullUpdate();
    }

signals:
    void changed();

private:
    QList<QObject *> items_;
};

#endif // QOBJECTLISTMODEL_H
