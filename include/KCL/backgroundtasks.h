#ifndef BACKGROUNDTASKS_H_INCLUDED
#define BACKGROUNDTASKS_H_INCLUDED

#include <QMutex>
#include <QList>
#include <QEventLoop>
#include <QThread>
#include <QThreadPool>
#include <QWaitCondition>

#include <QMap>
#include <QTimerEvent>

class Task : public QObject, public QRunnable
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

class EventLoopTask : public Task
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

class TaskProcessingController : public QObject
{
    Q_OBJECT
public:
    TaskProcessingController(QObject *parent = 0);
	virtual ~TaskProcessingController();
	
    void addTask(Task *task);
    bool removeTask(Task *task);
    bool isTaskEnqueued(Task *task);

    bool isIdle();
    void waitForDone();
	
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
