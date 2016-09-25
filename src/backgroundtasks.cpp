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

#include <QThread>
#include <QThreadPool>

#include <QCoreApplication>
#include <QTimer>

#include "KCL/backgroundtasks.h"

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

/* Task */

Task::Task() :
    QObject(NULL),
    QRunnable()
{
    setAutoDelete(true);
}

Task::~Task()
{
}

void Task::run()
{
    DGUARDMETHODTIMED;

    emit started(this, QThread::currentThread());
    execute();
    emit finished(this);
}

void Task::postExecute()
{
}

/* InterruptableTask */

InterruptableTask::InterruptableTask() :
    Task(),
    stopped_(false)
{
}

InterruptableTask::~InterruptableTask()
{
}

void InterruptableTask::requestStop()
{
    stopped_ = true;
}

bool InterruptableTask::stopRequested() const
{
    return stopped_;
}

/* EventLoopTask */

EventLoopTask::EventLoopTask() :
    Task(),
    eventLoop_(NULL)
{
}

EventLoopTask::~EventLoopTask()
{
}

void EventLoopTask::execute()
{
    DGUARDMETHOD;
    eventLoop_ = new QEventLoop(this);
    eventLoop_->exec();
}

void EventLoopTask::quit()
{
    if (eventLoop_)
        eventLoop_->quit();
}


/* TaskProcessingController */

TaskProcessingController::TaskProcessingController(QObject *parent, bool useGlobalThreadPool)
    :	QObject(parent)
{
    DGUARDMETHODTIMED;

    if (useGlobalThreadPool)
        pool_ = QThreadPool::globalInstance();
    else
    {
        pool_ = new QThreadPool(this);
        pool_->setMaxThreadCount(QThread::idealThreadCount() * 2);
    }
}

TaskProcessingController::~TaskProcessingController()
{
    DGUARDMETHODTIMED;
	
	// Clear the queues...
	clear();
	
	// Signal sleeping threads to wake up and discover they are marked for termination...
    waitForDone();
}

void TaskProcessingController::clear()
{
    qDeleteAll(taskQueue_);
    taskQueue_.clear();
}

void TaskProcessingController::handleTaskStarted(Task *task, QThread *thread)
{
    DGUARDMETHODTIMED;
    task->moveToThread(thread);
}

void TaskProcessingController::handleTaskFinished(Task *task)
{
    DGUARDMETHODTIMED;
    task->postExecute();
}

void TaskProcessingController::addTask(Task *task)
{
    DGUARDMETHODTIMED;

    DPRINTF("Adding task %p", task);
	
    connect(task, SIGNAL(started(Task*, QThread*)), this, SLOT(handleTaskStarted(Task*, QThread*)), Qt::BlockingQueuedConnection);
    connect(task, SIGNAL(finished(Task*)), this, SLOT(handleTaskFinished(Task*)), Qt::BlockingQueuedConnection);
    connect(task, SIGNAL(destroyed()), this, SLOT(tryStartNextTask()), Qt::QueuedConnection);

    taskQueue_.append(task);

    tryStartNextTask();
}

bool TaskProcessingController::removeTask(Task *task)
{
    return taskQueue_.removeAll(task) > 0;
}

bool TaskProcessingController::isTaskEnqueued(Task *task)
{
    return taskQueue_.contains(task);
}

bool TaskProcessingController::isIdle()
{
    return pool_->activeThreadCount() == 0;
}

void TaskProcessingController::tryStartNextTask()
{
    DGUARDMETHODTIMED;

    bool enqueued = true;

    while (taskQueue_.length() > 0 && enqueued)
    {
        Task *task = taskQueue_.takeFirst();

        enqueued = pool_->tryStart(task);

        if (!enqueued)
            taskQueue_.prepend(task);
    }

    DPRINTF("Tasks left in queue: %d, active threads: %d, max threads: %d", taskQueue_.length(), pool_->activeThreadCount(), pool_->maxThreadCount());
}

void TaskProcessingController::waitForDone(int maxWaitInMs)
{
    QEventLoop loop;
    QElapsedTimer timer;
    timer.start();

    while (taskQueue_.length() > 0 || pool_->activeThreadCount() > 0)
    {
        tryStartNextTask();
        loop.processEvents(QEventLoop::ExcludeUserInputEvents);

        if (maxWaitInMs > -1 && timer.elapsed() > maxWaitInMs)
            break;
    }

    // NOTE: We can't use QThreadPool::waitForDone here because the
    // BlockingQueuedConnection of the finished signal will cause a deadlock.
}
