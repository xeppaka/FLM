#ifndef LASERTHREAD_H
#define LASERTHREAD_H

#include "falconoutputdevice.h"
#include <QThread>

class LaserThread : public QThread
{
    Q_OBJECT
public:
    LaserThread(FalconOutputDevice *fod);
    void run();
    void interrupt();

    bool mResult;

private:
    FalconOutputDevice *mFOD;

signals:
    void finished(bool result);
};

#endif // LASERTHREAD_H
