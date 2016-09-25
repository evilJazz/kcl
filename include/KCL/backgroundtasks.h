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

#ifndef BACKGROUNDTASKS_H_INCLUDED
#define BACKGROUNDTASKS_H_INCLUDED

#include "kcl_global.h"

#include <QMutex>
#include <QList>
#include <QEventLoop>
#include <QThread>
#include <QThreadPool>
#include <QWaitCondition>

#include <QMap>
#include <QTimerEvent>

class KCL_EXPORT Task : public QObject, public QRunnable
{
    Q_OBJECT
public:
	Task();
    virtual ~Task();

    virtual void execute() = 0;
    virtual void postExecute();

signals:
    void started(Task *task, QThread *thread);
    void finished(Task *task);

private:
    friend class TaskProcessingController;
    virtual void run();
};

class KCL_EXPORT InterruptableTask : public Task
{
public:
    InterruptableTask();
    virtual ~InterruptableTask();

    void requestStop();
    bool stopRequested() const;

private:
    bool stopped_;
};

class KCL_EXPORT EventLoopTask : public Task
{
    Q_OBJECT
public:
    EventLoopTask();
    virtual ~EventLoopTask();

    virtual void execute();

public slots:
    void quit();

private:
    QEventLoop *eventLoop_;
};

class KCL_EXPORT TaskProcessingController : public QObject
{
    Q_OBJECT
public:
    TaskProcessingController(QObject *parent = 0, bool useGlobalThreadPool = false);
	virtual ~TaskProcessingController();
	
    void addTask(Task *task);
    bool removeTask(Task *task);
    bool isTaskEnqueued(Task *task);

    bool isIdle();
    void waitForDone(int maxWaitInMs = -1);
	
	void clear();

private slots:
    void handleTaskStarted(Task *task, QThread *thread);
    void handleTaskFinished(Task *task);
    void tryStartNextTask();

protected:
    QThreadPool *pool_;
    QList<Task *> taskQueue_;
};

#endif /*BACKGROUNDTASKS_H_INCLUDED*/
