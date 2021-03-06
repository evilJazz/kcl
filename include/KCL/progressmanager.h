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

#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>

class ProgressManager;
class ProgressContext;
class ProgressActivityGuard;

typedef QList<ProgressContext *> ProgressContextList;

class KCL_EXPORT ProgressContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString progressText READ progressText WRITE setProgressText NOTIFY progressTextChanged)
    Q_PROPERTY(bool progressIsAbsolute READ progressIsAbsolute WRITE setProgressIsAbsolute NOTIFY progressIsAbsoluteChanged)
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    Q_PROPERTY(int progressTotal READ progressTotal WRITE setProgressTotal NOTIFY progressTotalChanged)
    Q_PROPERTY(qreal progressPercentage READ progressPercentage NOTIFY progressPercentageChanged)
    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(bool cancelled READ cancelled NOTIFY cancelledChanged)
    Q_PROPERTY(QString activityName READ activityName CONSTANT)
public:
    explicit ProgressContext(ProgressContext *parent, const QString &activityName, int countSubSteps = 0);
    virtual ~ProgressContext();

    QString activityName() const { return activityName_; }

    qreal progressPercentage() const;
    bool isGroup() const;

    Q_INVOKABLE void setProgressIsAbsolute(bool value);
    Q_INVOKABLE void setProgressText(const QString &text);
    Q_INVOKABLE void setProgressValue(int value);
    Q_INVOKABLE void setProgressTotal(int total);
    Q_INVOKABLE void setData(const QVariant &data);

    bool progressIsAbsolute() const { return progressIsAbsolute_; }
    const QString &progressText() const { return progressText_; }
    int progressValue() const { return progressValue_; }
    int progressTotal() const { return progressTotal_; }
    QVariant data() const { return data_; }

    Q_INVOKABLE void setBias(int subStep, float bias);
    qreal bias(int subStep);

    bool cancelled() const { return cancelled_; }
    Q_INVOKABLE void cancel();

    void dumpActivities(int indentLevel = 0);

signals:
    void progressIsAbsoluteChanged();
    void progressTextChanged();
    void progressValueChanged();
    void progressTotalChanged();
    void progressPercentageChanged();
    void dataChanged();
    void cancelledChanged();

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

    bool progressIsAbsolute_;
    QString progressText_;
    int progressValue_;
    int progressTotal_;
    QVariant data_;

    bool cancelled_;
};

class KCL_EXPORT ProgressManager : public QObject
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
    Q_INVOKABLE void updateActivity(const QString &progressText, int progressValue, int progressTotal, bool isAbsolute = false);
    Q_INVOKABLE void updateActivity(int progressValue, int progressTotal, bool isAbsolute = false);

    Q_INVOKABLE void endActivity();

    Q_INVOKABLE void asyncTick(int progressValue, int progressTotal);

    Q_INVOKABLE void dumpTopLevelContext();

    Q_INVOKABLE ProgressContext *topLevelContext() { return topLevelContext_; }
    Q_INVOKABLE ProgressContext *currentContext() { return currentContext_; }

signals:
    void topLevelContextChanged();
    void currentContextChanged();

private:
    QList<ProgressContext *> contexts_;

    ProgressContext *topLevelContext_;
    ProgressContext *currentContext_;

    void setCurrentContext(ProgressContext *context);
    void setTopLevelContext(ProgressContext *context);
};

class KCL_EXPORT ProgressActivityGuard
{
public:
    explicit ProgressActivityGuard(int subSteps = 0, const QString &name = "Activity") :
        subSteps_(subSteps), theGreatOptimizationPreventer_(name), done_(false)
    {
        globalProgressManager.beginActivity(name, subSteps);
    }

    ~ProgressActivityGuard()
    {
        globalProgressManager.endActivity();
    }

    inline void setBias(int subStep, qreal bias) { globalProgressManager.setActivityBias(subStep, bias); }
    inline void update(const QString &progressText, int progressValue, int progressTotal) { globalProgressManager.updateActivity(progressText, progressValue, progressTotal); }
    inline void update(int progressValue, int progressTotal) { globalProgressManager.updateActivity(progressValue, progressTotal); }

    inline ProgressContext *context() { return globalProgressManager.currentContext(); }
    inline ProgressContext *topLevelContext() { return globalProgressManager.topLevelContext(); }

    inline bool done() { return done_; }
    inline void stop() { done_ = true; }

private:
    int subSteps_;
    QString theGreatOptimizationPreventer_;
    bool done_;
};

#define ScopedActivity() volatile ProgressActivityGuard scopedActivity(0, "Activity")
#define ScopedActivityWithSubSteps(subSteps) volatile ProgressActivityGuard scopedActivityWithSubSteps(subSteps, "Activity")
#define NamedScopedActivityWithSubSteps(name, subSteps) volatile ProgressActivityGuard name(subSteps, "Activity")

// Syntactical sugar
#define activity(...) for (ProgressActivityGuard activity(__VA_ARGS__); !activity.done(); activity.stop())

/* USAGE:

activity (0) // no sub step
{
    // Your code...
    activity.update(50, 100); // set progress to 50%
    // Your code...
}

*/

#endif // PROGRESSMANAGER_H
