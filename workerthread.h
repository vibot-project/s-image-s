#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "init.h"
#include "workingcontext.h"
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QTime>

class WorkerThread{
public:
    WorkerThread();
};
#endif // WORKERTHREAD_H
