#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "init.h"
#include <QMutex>

class WorkerThread
{
private:
    QMutex mute;
public:
    WorkerThread();
};

#endif // WORKERTHREAD_H
