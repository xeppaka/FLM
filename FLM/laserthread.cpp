#include "stdheader.h"
#include "laserthread.h"

LaserThread::LaserThread(FalconOutputDevice *fod)
{
    mFOD = fod;
}

void LaserThread::run()
{
    mResult = mFOD->flush();
}

void LaserThread::interrupt()
{
    if (isRunning())
        mFOD->interrupt();
}
