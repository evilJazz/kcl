#ifndef SLEEPAID_H
#define SLEEPAID_H

#include <QThread>

class SleepAid : public QThread
{
public:
    static void sleep(unsigned long secs) { QThread::sleep(secs); }
    static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
    static void usleep(unsigned long usecs) { QThread::usleep(usecs); }
};

#endif // SLEEPAID_H