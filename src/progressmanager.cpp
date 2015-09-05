/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "KCL/progressmanager.h"
#include <QCoreApplication>

#include <QThread>
#include <QEventLoop>

#include "KCL/debug.h"

static ProgressManager *progressMan = NULL;

/* ProgressContext */

ProgressContext::ProgressContext(ProgressContext *parent, const QString &activityName, int countSubSteps) :
    QObject(parent),
    activityName_(activityName),
    parent_(parent),
    expectedSubSteps_(countSubSteps),
    progressText_(""),
    progressValue_(0),
    progressTotal_(1),
    cancelled_(false)
{
    for (int i = 0; i < countSubSteps; ++i)
        biases_.append(1); // all items are equal by default.

    connect(this, SIGNAL(progressValueChanged()), this, SIGNAL(progressPercentageChanged()));
    connect(this, SIGNAL(progressTotalChanged()), this, SIGNAL(progressPercentageChanged()));
}

ProgressContext::~ProgressContext()
{
}

qreal ProgressContext::progressPercentage() const
{
    if (!isGroup())
    {
        return progressTotal_ == 0 ? 0 : (qreal)progressValue_ / (qreal)progressTotal_;
    }
    else
    {
        qreal percentage = 0;
        qreal biasTotal = 0;

        for (int i = 0; i < biases_.count(); ++i)
        {
            qreal bias = biases_.at(i);
            biasTotal += bias;

            if (i < subSteps_.count())
                percentage += subSteps_.at(i)->progressPercentage() * bias;
        }

        return percentage / biasTotal;
    }
}

bool ProgressContext::isGroup() const
{
    return expectedSubSteps_ > 0;
}

void ProgressContext::setProgressIsAbsolute(bool value)
{
    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setProgressIsAbsolute",
            Qt::BlockingQueuedConnection,
            Q_ARG(bool, value)
        );
    }
    else
    {
        if (value != progressIsAbsolute_)
        {
            progressIsAbsolute_ = value;
            emit progressIsAbsoluteChanged();
        }
    }
}

void ProgressContext::setProgressText(const QString &text)
{
    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setProgressText",
            Qt::BlockingQueuedConnection,
            Q_ARG(const QString &, text)
        );
    }
    else
    {
        if (text != progressText_)
        {
            progressText_ = text;
            emit progressTextChanged();
        }
    }
}

void ProgressContext::setProgressValue(int value)
{
    DASSERT(!isGroup(), "Can not change progress value for a activity group.");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setProgressValue",
            Qt::BlockingQueuedConnection,
            Q_ARG(int, value)
        );
    }
    else
    {
        if (value != progressValue_)
        {
            progressValue_ = value;
            emit progressValueChanged();
        }
    }
}

void ProgressContext::setProgressTotal(int total)
{
    DASSERT(!isGroup(), "Can not change progress total for a activity group.");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setProgressTotal",
            Qt::BlockingQueuedConnection,
            Q_ARG(int, total)
        );
    }
    else
    {
        if (total != progressTotal_)
        {
            progressTotal_ = total;
            emit progressTotalChanged();
        }
    }
}

void ProgressContext::setData(const QVariant &data)
{
    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setData",
            Qt::BlockingQueuedConnection,
            Q_ARG(const QVariant &, data)
        );
    }
    else
    {
        if (data != data_)
        {
            data_ = data;
            emit dataChanged();
        }
    }
}

void ProgressContext::setBias(int subStep, float bias)
{
    DASSERT(subSteps_.count() == 0, "Do not change bias in a group once activity in it has started.");
    DASSERT(subStep > -1 && subStep < biases_.count(), "Bias index does not exist for this activity.");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setBias",
            Qt::BlockingQueuedConnection,
            Q_ARG(int, subStep),
            Q_ARG(float, bias)
        );
    }
    else
    {
        if (subStep > -1 && subStep < biases_.count())
        {
            if (bias != biases_.at(subStep))
            {
                biases_[subStep] = bias;
                emit progressPercentageChanged();
            }
        }
    }
}

qreal ProgressContext::bias(int subStep)
{
    return biases_.at(subStep);
}

void ProgressContext::cancel()
{
    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "cancel",
            Qt::BlockingQueuedConnection
        );
    }
    else
    {
        if (!cancelled_)
        {
            cancelled_ = true;
            emit cancelledChanged();
        }
    }
}

void ProgressContext::dumpActivities(int indentLevel)
{
    if (isGroup())
    {
        qDebug("%*s Activity Group of %d expected items (Completion: %.2f)", indentLevel * 3, "", expectedSubSteps_, progressPercentage());

        for (int i = 0; i < subSteps_.count(); ++i)
        {
            qDebug("%*s %d: Bias: %.2f", indentLevel * 3, "", i, biases_.at(i));
            subSteps_.at(i)->dumpActivities(indentLevel + 1);
        }
    }
    else
    {
        qDebug("%*s Activity", indentLevel * 3, "");
        qDebug("%*s Text: %s", indentLevel * 3, "", progressText_.toUtf8().constData());
        qDebug("%*s Value: %d of %d (Completion: %.2f)", indentLevel * 3, "", progressValue_, progressTotal_, progressPercentage());
    }
}

ProgressContext *ProgressContext::beginActivity(const QString &activityName, int countSubSteps)
{
    DASSERT(isGroup(), "Can not add a new activity to an existing activity that can not contain sub-activities!");
    DASSERT(subSteps_.count() < expectedSubSteps_, "You are adding more substeps than have been initially defined!");

    ProgressContext *result = new ProgressContext(this, activityName, countSubSteps);
    subSteps_.append(result);
    connect(result, SIGNAL(progressPercentageChanged()), this, SIGNAL(progressPercentageChanged()));
    return result;
}

ProgressContext *ProgressContext::endActivity(ProgressContext *activity)
{
    DASSERT(subSteps_.contains(activity), "Can not end an activity that is not in this group. Unpaired beginActivity <-> endActivity !!!");

    if (!activity->isGroup())
        activity->setProgressValue(activity->progressTotal());

    //if (subSteps_.count() == expectedSubSteps_)
    //    return parent_;
    //else
        return this;
}

/* ProgressManager */

ProgressManager::ProgressManager(QObject *parent) :
    QObject(parent),
    topLevelContext_(NULL),
    currentContext_(NULL)
{
    DGUARDMETHODTIMED;
}

ProgressManager::~ProgressManager()
{
    DGUARDMETHODTIMED;
    qDeleteAll(contexts_);
}

ProgressManager &ProgressManager::singleton()
{
    if (!progressMan)
        progressMan = new ProgressManager(qApp);

    return *progressMan;
}

void ProgressManager::beginActivity(const QString &activityName, int subSteps)
{
    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
                this, "beginActivity",
                Qt::BlockingQueuedConnection,
                Q_ARG(const QString &, activityName),
                Q_ARG(int, subSteps)
        );
    }
    else
    {
        ProgressContext *activity;

        if (!topLevelContext_ || !currentContext_)
        {
            activity = new ProgressContext(NULL, activityName, subSteps);
            contexts_.append(activity);
            setTopLevelContext(activity);
        }
        else
            activity = currentContext_->beginActivity(activityName, subSteps);

        setCurrentContext(activity);
    }
}

void ProgressManager::setActivityBias(int subStep, qreal bias)
{
    DASSERT(currentContext_ != NULL && currentContext_->isGroup(), "Start an activity group first before you can set bias on it...");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "setActivityBias",
            Qt::BlockingQueuedConnection,
            Q_ARG(int, subStep),
            Q_ARG(qreal, bias)
        );
    }
    else
    {
        if (currentContext_)
            currentContext_->setBias(subStep, bias);
    }
}

void ProgressManager::updateActivity(const QString &progressText, int progressValue, int progressTotal, bool isAbsolute)
{
    DASSERT(currentContext_ != NULL, "Start an activity first before you can update it...");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "updateActivity",
            Qt::BlockingQueuedConnection,
            Q_ARG(const QString &, progressText),
            Q_ARG(int, progressValue),
            Q_ARG(int, progressTotal),
            Q_ARG(bool, isAbsolute)
        );
    }
    else
    {
        if (currentContext_)
        {
            currentContext_->setProgressIsAbsolute(isAbsolute);
            currentContext_->setProgressText(progressText);
            currentContext_->setProgressTotal(progressTotal);
            currentContext_->setProgressValue(progressValue);
        }
    }
}

void ProgressManager::updateActivity(int progressValue, int progressTotal, bool isAbsolute)
{
    DASSERT(currentContext_ != NULL, "Start an activity first before you can update it...");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "updateActivity",
            Qt::BlockingQueuedConnection,
            Q_ARG(int, progressValue),
            Q_ARG(int, progressTotal),
            Q_ARG(bool, isAbsolute)
        );
    }
    else
    {
        if (currentContext_)
        {
            currentContext_->setProgressIsAbsolute(isAbsolute);
            currentContext_->setProgressTotal(progressTotal);
            currentContext_->setProgressValue(progressValue);
        }
    }
}

void ProgressManager::endActivity()
{
    DASSERT(currentContext_ != NULL, "Start an activity first before you can end it...");

    if (QThread::currentThread() != this->thread())
    {
        QMetaObject::invokeMethod(
            this, "endActivity",
            Qt::BlockingQueuedConnection
        );
    }
    else
    {
        if (currentContext_)
        {
            if (currentContext_->parent_)
                setCurrentContext(currentContext_->parent_->endActivity(currentContext_));
            else
            {
                if (!currentContext_->isGroup())
                    currentContext_->setProgressValue(currentContext_->progressTotal());

                setCurrentContext(NULL);
            }
        }

        if (currentContext_ == NULL)
        {
            setTopLevelContext(NULL);
            // TODO: EMIT DONE!
        }
    }
}

void ProgressManager::asyncTick(int progressValue, int progressTotal)
{
    QEventLoop el;
    el.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void ProgressManager::dumpTopLevelContext()
{
    if (topLevelContext_)
        topLevelContext_->dumpActivities();
}

void ProgressManager::setCurrentContext(ProgressContext *context)
{
    currentContext_ = context;
    emit currentContextChanged();
}

void ProgressManager::setTopLevelContext(ProgressContext *context)
{
    topLevelContext_ = context;
    emit topLevelContextChanged();
}
