#ifndef PERFORMANCEDATAMANAGER_H
#define PERFORMANCEDATAMANAGER_H

#include <QObject>
#include <QVariant>
#include <QHash>

class PerformanceDataManager;
class PerformanceData;

class PerformanceDataDetail : public QObject
{
    Q_OBJECT
public:
    QString identifier;
    qint64 startTimeStamp;
    qint64 stopTimeStamp;

    QVariantMap asVariant();
};

class PerformanceDataAspect : public QObject
{
    Q_OBJECT
public:
    PerformanceDataAspect(PerformanceData *parent, const QString &identifier);
    virtual ~PerformanceDataAspect();

    Q_INVOKABLE PerformanceDataDetail *startDetail(const QString &identifier);
    Q_INVOKABLE PerformanceDataDetail *stopDetail(const QString &identifier);

    Q_INVOKABLE qint64 startTimeStamp() const { return startTimeStamp_; }
    Q_INVOKABLE qint64 endTimeStamp() const { return endTimeStamp_; }

    Q_INVOKABLE QVariantMap asVariant();

private:
    PerformanceData *parent_;
    QString identifier_;
    qint64 startTimeStamp_;
    qint64 endTimeStamp_;
    QList<PerformanceDataDetail *> details_;
    QHash<QString, PerformanceDataDetail *> detailHash_;
};

class PerformanceData : public QObject
{
    Q_OBJECT
public:
    PerformanceData(PerformanceDataManager *parent, const QString &name);
    virtual ~PerformanceData();

    Q_INVOKABLE PerformanceDataAspect *aspect(const QString &identifier);
    Q_INVOKABLE void deleteAspect(const QString &identifier);

    Q_INVOKABLE void finish();

    Q_INVOKABLE QVariantMap asVariant();

private:
    friend class PerformanceDataManager;
    PerformanceDataManager *parent_;
    QString name_;
    QList<PerformanceDataAspect *> aspects_;
    QHash<QString, PerformanceDataAspect *> aspectHash_;
};

class PerformanceDataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool keepData READ keepData WRITE setKeepData NOTIFY keepDataChanged)
public:
    explicit PerformanceDataManager(QObject *parent = 0);
    virtual ~PerformanceDataManager();

#define qPerformanceDataManager PerformanceDataManager::singleton()
    static PerformanceDataManager &singleton();

    Q_INVOKABLE PerformanceData *addNewPerformanceData(const QString &name = QString::null);

    Q_INVOKABLE PerformanceData *data(const QString &name);
    Q_INVOKABLE PerformanceData *at(int index) { return perfDataList_[index]; }
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const { return perfDataList_.count(); }
    Q_INVOKABLE void clear();

    Q_INVOKABLE QVariant asVariant(int index = -1);

    bool keepData() const { return keepData_; }
    void setKeepData(bool value);

    // Shortcut methods:
    Q_INVOKABLE PerformanceDataDetail* startDetail(const QString &dataName, const QString &aspectId, const QString &detailId);
    Q_INVOKABLE PerformanceDataDetail* stopDetail(const QString &dataName, const QString &aspectId, const QString &detailId);

signals:
    void dataFinished(PerformanceData *data);
    void keepDataChanged();

protected:
    friend class PerformanceData;
    void emitDataFinished(PerformanceData *data);

public:
    bool keepData_;
    QList<PerformanceData *> perfDataList_;
    QHash<QString, PerformanceData *> perfDataHash_;
};

#define PERFDATA_STARTDETAIL(dataId, aspectId, detailId) qPerformanceDataManager.startDetail(dataId, aspectId, detailId)
#define PERFDATA_STOPDETAIL(dataId, aspectId, detailId) qPerformanceDataManager.stopDetail(dataId, aspectId, detailId)

#define cStartUp "Start Up"

#endif // PERFORMANCEDATAMANAGER_H
