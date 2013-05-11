#ifndef GRAPHICSOUTPUTDEVICE_H
#define GRAPHICSOUTPUTDEVICE_H

#include <QGraphicsView>
#include <QGraphicsScene>

#include "outputdevice.h"

#define LASER_ON_COLOR Qt::red
#define LASER_OFF_COLOR Qt::lightGray
#define OUTFIELD_ELEMENTS_COLOR Qt::black

#define FIELD_PADDING 5000

enum Render_Mode
{
    FAST = 0,
    STEPS,
    LINES
};

enum Previous_Coordinate
{
    NONE = 0,
    X,
    Y
};

enum Previous_Laser_State
{
    ON = 0,
    OFF
};

class GraphicsOutputDevice : public OutputDevice
{
public:
    GraphicsOutputDevice(QGraphicsView *graphics);
    ~GraphicsOutputDevice();

private:
    QGraphicsView *mGraphics;
    QGraphicsScene *mGraphicsScene;
    QPainterPath *mPainterPathLaserOn;
    QPainterPath *mPainterPathLaserOff;
    QPen mPenLaserOn;
    QPen mPenLaserOff;
    QPen mPenOutField;
    int mCurrPosX,mCurrPosY;
    Render_Mode mRenderMode;
    bool mDrawIdleMotion;
    int mPreviousCoord;
    Previous_Coordinate mPreviousCoordType;
    Previous_Laser_State mPreviousLaserState;

    void drawOutfieldElements();

public:
    virtual bool begin(MarkDelays *delays);
    virtual bool end();
    virtual bool flush();
    virtual void interrupt();

    // High level functions for drawing
//    bool line();
//    bool conic();
//    bool cubic();


    // Low level drawing functions. Real device imitation.
    virtual bool writeXYAndLaserOff(int x, int y);
    virtual bool writeXYAndLaserOn(int x, int y);
    virtual bool writeXAndLaserOffDelay(int x);
    virtual bool writeXAndLaserOnDelay(int x);
    virtual bool writeXAndLaserOff(int x);
    virtual bool writeXAndLaserOn(int x);
    virtual bool writeYAndLaserOffDelay(int y);
    virtual bool writeYAndLaserOnDelay(int y);
    virtual bool writeYAndLaserOff(int y);
    virtual bool writeYAndLaserOn(int y);

    /* Permanent delay. command: 20 00 TTTT      */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writePermDelay(int delay);

    /* One time delay. command: 20 04 TTTT       */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writeOnceDelay(int delay);

    virtual bool writeLaserOnDelay(int delay);
    virtual bool writeLaserOffDelay(int delay);

    virtual bool writeQSWPeriod(int delay);
    virtual bool writeQSWDuration(int delay);

    void setRenderMode(Render_Mode newMode, bool drawIdleMotion);

    // this functions are supported only by this class
    virtual bool moveTo(int x, int y);

    virtual bool lineTo(int x, int y);

    // Beizer arcs
    virtual bool conicTo(int controlX, int controlY,
                         int toX, int toY);

    virtual bool cubicTo(int control1X, int control1Y,
                         int control2X, int control2Y,
                         int toX, int toY);
    virtual bool segmentTo(int centerX, int centerY, int toX, int toY);
};

#endif // GRAPHICSOUTPUTDEVICE_H
