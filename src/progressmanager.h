#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>

class ProgressManager;
class ProgressContext;
class ProgressActivityGuard;

typedef QList<ProgressContext *> ProgressContextList;

class ProgressContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString progressText READ progressText WRITE setProgressText NOTIFY progressTextChanged)
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    Q_PROPERTY(int progressTotal READ progressTotal WRITE setProgressTotal NOTIFY progressTotalChanged)
    Q_PROPERTY(qreal progressPercentage READ progressPercentage() NOTIFY progressPercentageChanged)
    Q_PROPERTY(QString activityName READ activityName CONSTANT)
public:
    explicit ProgressContext(ProgressContext *parent, const QString &activityName, int countSubSteps = 0);
    virtual ~ProgressContext();

    QString activityName() const { return activityName_; }

    qreal progressPercentage() const;
    bool isGroup() const;

    void setProgressText(const QString &text);
    void setProgressValue(int value);
    void setProgressTotal(int total);

    const QString &progressText() const { return progressText_; }
    int progressValue() const { return progressValue_; }
    int progressTotal() const { return progressTotal_; }

    void setBias(int subStep, float bias);
    qreal bias(int subStep);

    bool cancelled() { return false; } // TODO

    void dumpActivities(int indentLevel = 0);

signals:
    void progressTextChanged();
    void progressValueChanged();
    void progressTotalChanged();
    void progressPercentageChanged();

protected:
    friend class ProgressManager;
    ProgressContext *beginActivity(const QString &activityName, int countSubSteps = 0);
    ProgressContext *endActivity(ProgressContext *activity);

private:
    QString activityName_;

    ProgressContextList subSteps_;
    QList<qreal> biases_;

    ProgressContext *parent_;
    int expectedSubSteps_;

    QString progressText_;
    int progressValue_;
    int progressTotal_;
};

class ProgressManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ProgressContext *currentContext READ currentContext NOTIFY currentContextChanged)
    Q_PROPERTY(ProgressContext *topLevelContext READ topLevelContext NOTIFY topLevelContextChanged)
public:
    explicit ProgressManager(QObject *parent = 0);
    virtual ~ProgressManager();

    static ProgressManager& singleton();

#   define globalProgressManager ProgressManager::singleton()

    Q_INVOKABLE void beginActivity(const QString &activityName, int subSteps = 0);

    Q_INVOKABLE void setActivityBias(int subStep, qreal bias);
    Q_INVOKABLE void updateActivity(const QString &progressText, int progressValue, int progressTotal);
    Q_INVOKABLE void updateActivity(int progressValue, int progressTotal);

    Q_INVOKABLE void endActivity();

    Q_INVOKABLE void asyncTick(int progressValue, int progressTotal);

    Q_INVOKABLE void dumpTopLevelContext();

    Q_INVOKABLE ProgressContext *topLevelContext() { return topLevelContext_; }
    Q_INVOKABLE ProgressContext *currentContext() { return currentContext_; }

signals:
    void topLevelContextChanged();
    void currentContextChanged();

private slots:
    void progressPercentageChangedHandler();

private:
    QList<ProgressContext *> contexts_;

    ProgressContext *topLevelContext_;
    ProgressContext *currentContext_;

    void setCurrentContext(ProgressContext *context);
    void setTopLevelContext(ProgressContext *context);
};

class ProgressActivityGuard
{
public:
    ProgressActivityGuard(int subSteps = 0, const QString &name = "Activity") :
        subSteps_(subSteps), theGreatOptimizationPreventer_(name)
    {
        globalProgressManager.beginActivity(name, subSteps);
    }

    virtual ~ProgressActivityGuard()
    {
        globalProgressManager.endActivity();
    }

private:
    int subSteps_;
    QString theGreatOptimizationPreventer_;
};

#define ScopedActivity() volatile ProgressActivityGuard scopedActivity(0, "Activity")
#define ScopedActivityWithSubSteps(subSteps) volatile ProgressActivityGuard scopedActivityWithSubSteps(subSteps, "Activity")
#define NamedScopedActivityWithSubSteps(name, subSteps) volatile ProgressActivityGuard name(subSteps, "Activity")

#endif // PROGRESSMANAGER_H
