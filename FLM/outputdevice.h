#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include "stdheader.h"
#include "markdelays.h"

#define FIELD_WIDTH 65535
#define FIELD_HEIGHT 65535

#define START_POS_X FIELD_WIDTH/2
#define START_POS_Y FIELD_HEIGHT/2

#define HORZ_RESOLUTION 28800
#define VERT_RESOLUTION 28800

#define PI 3.1415926535897932384626433832795

class OutputDevice
{
public:
    virtual ~OutputDevice(){}

public:
    virtual bool writeXYAndLaserOff(int x, int y) = 0;
    virtual bool writeXYAndLaserOn(int x, int y) = 0;
    virtual bool writeXAndLaserOffDelay(int x) = 0;
    virtual bool writeXAndLaserOnDelay(int x) = 0;
    virtual bool writeXAndLaserOff(int x) = 0;
    virtual bool writeXAndLaserOn(int x) = 0;
    virtual bool writeYAndLaserOffDelay(int y) = 0;
    virtual bool writeYAndLaserOnDelay(int y) = 0;
    virtual bool writeYAndLaserOff(int y) = 0;
    virtual bool writeYAndLaserOn(int y) = 0;

    /* Permanent delay. command: 20 00 TTTT      */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writePermDelay(int delay) = 0;

    /* One time delay. command: 20 04 TTTT       */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writeOnceDelay(int delay) = 0;

    virtual bool writeLaserOnDelay(int delay) = 0;
    virtual bool writeLaserOffDelay(int delay) = 0;

    virtual bool writeQSWPeriod(int delay) = 0;
    virtual bool writeQSWDuration(int delay) = 0;

    virtual bool begin(MarkDelays *delays) = 0;
    virtual bool end() = 0;
    virtual bool flush() = 0;
    virtual void interrupt() = 0;

    // fast high level functions for fast drawing on the screen
    virtual bool moveTo(int x, int y) = 0;

    virtual bool lineTo(int x, int y) = 0;

    // Beizer arcs
    virtual bool conicTo(int controlX, int controlY,
                 int toX, int toY) = 0;

    virtual bool cubicTo(int control1X, int control1Y,
                 int control2X, int control2Y,
                 int toX, int toY) = 0;
    virtual bool segmentTo(int centerX, int centerY, int toX, int toY) = 0;
};

#endif // OUTPUTDEVICE_H
