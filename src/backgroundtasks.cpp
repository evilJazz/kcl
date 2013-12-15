#include <QThread>
#include <QThreadPool>

#include <QCoreApplication>
#include <QTimer>

#include "KCL/backgroundtasks.h"
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

    //moveToThread(QThread::currentThread());

    execute();
    emit finished(this);
}

void Task::postExecute()
{
}


/* TaskProcessingController */

TaskProcessingController::TaskProcessingController(QObject *parent)
    :	QObject(parent)
{
    DGUARDMETHODTIMED;

    pool_ = new QThreadPool(this);
    pool_->setMaxThreadCount(QThread::idealThreadCount() * 2);
}

TaskProcessingController::~TaskProcessingController()
{
    DGUARDMETHODTIMED;
	
	// Clear the queues...
	clear();
	
	// Signal sleeping threads to wake up and discover they are marked for termination...
    pool_->waitForDone();
}

void TaskProcessingController::clear()
{
    qDeleteAll(taskQueue_);
    taskQueue_.clear();
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


void TaskProcessingController::waitForDone()
{
    QEventLoop loop;

    while (taskQueue_.length() > 0 || pool_->activeThreadCount() > 0)
    {
        tryStartNextTask();
        loop.processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    // NOTE: We can't use QThreadPool::waitForDone here because the
    // BlockingQueuedConnection of the finished signal will cause a deadlock.
}
