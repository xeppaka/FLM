#include "graphicsoutputdevice.h"

GraphicsOutputDevice::GraphicsOutputDevice(QGraphicsView *graphics)
{
    mGraphics = graphics;
    mRenderMode = FAST;
    mDrawIdleMotion = false;
    mGraphicsScene = new QGraphicsScene(-FIELD_PADDING,
                                        -FIELD_PADDING,
                                        FIELD_WIDTH + FIELD_PADDING*2,
                                        FIELD_HEIGHT + FIELD_PADDING*2);

    mGraphics->setScene(mGraphicsScene);

    mPainterPathLaserOn = 0;
    mPainterPathLaserOff = 0;

    mPenLaserOn.setColor(LASER_ON_COLOR);
    mPenLaserOff.setColor(LASER_OFF_COLOR);
    mPenOutField.setColor(OUTFIELD_ELEMENTS_COLOR);
}

GraphicsOutputDevice::~GraphicsOutputDevice()
{
    mGraphics->setScene(0);
    if (mGraphicsScene)
        delete mGraphicsScene;
    if (mPainterPathLaserOn)
        delete mPainterPathLaserOn;
    if (mPainterPathLaserOff)
        delete mPainterPathLaserOff;
}

void GraphicsOutputDevice::setRenderMode(Render_Mode newMode, bool drawIdleMotion)
{
    mRenderMode = newMode;
    mDrawIdleMotion = drawIdleMotion;
}

void GraphicsOutputDevice::drawOutfieldElements()
{
    mGraphicsScene->addLine(-1,-1,-1,FIELD_HEIGHT+1,mPenOutField);
    mGraphicsScene->addLine(-1,FIELD_HEIGHT+1,FIELD_WIDTH+1,FIELD_HEIGHT+1,mPenOutField);
    mGraphicsScene->addLine(FIELD_WIDTH+1,FIELD_HEIGHT+1,FIELD_WIDTH+1,-1,mPenOutField);
    mGraphicsScene->addLine(FIELD_WIDTH+1,-1,-1,-1,mPenOutField);
}

bool GraphicsOutputDevice::writeYAndLaserOn(int y)
{
    if (mRenderMode == STEPS)
    {
        if (y != mCurrPosY)
        {
            mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOn);
            mCurrPosY = y;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            mPreviousCoordType = Y;
            mPreviousCoord = y;
            mPreviousLaserState = ON;
            break;
        case X:
            if (mCurrPosY != y || mCurrPosX != mPreviousCoord)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mPreviousCoord,y,mPenLaserOn);
                mCurrPosY = y;
                mCurrPosX = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case Y:
            writeXAndLaserOn(mCurrPosX);
            writeYAndLaserOn(y);
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeYAndLaserOnDelay(int y)
{
    if (mRenderMode == STEPS)
    {
        if (y != mCurrPosY)
        {
            mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOn);
            mCurrPosY = y;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            if (mCurrPosY != y)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOn);
                mCurrPosY = y;
            }
            break;
        case X:
            if (mCurrPosY != y || mCurrPosX != mPreviousCoord)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mPreviousCoord,y,mPenLaserOn);
                mCurrPosY = y;
                mCurrPosX = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case Y:
            writeXAndLaserOn(mCurrPosX);
            if (mCurrPosY != y)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOn);
                mCurrPosY = y;
            }
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeYAndLaserOff(int y)
{
    if (mRenderMode == STEPS)
    {
        if (y != mCurrPosY)
        {
            if (mDrawIdleMotion)
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOff);
            mCurrPosY = y;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            mPreviousCoordType = Y;
            mPreviousCoord = y;
            mPreviousLaserState = OFF;
            break;
        case X:
            if (mCurrPosY != y || mCurrPosX != mPreviousCoord)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mPreviousCoord,y,mPenLaserOff);
                mCurrPosY = y;
                mCurrPosX = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case Y:
            writeXAndLaserOff(mCurrPosX);
            writeYAndLaserOff(y);
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeYAndLaserOffDelay(int y)
{
    if (mRenderMode == STEPS)
    {
        if (y != mCurrPosY)
        {
            if (mDrawIdleMotion)
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOff);
            mCurrPosY = y;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            if (mCurrPosY != y)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOff);
                mCurrPosY = y;
            }
            break;
        case X:
            if (mCurrPosY != y || mCurrPosX != mPreviousCoord)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mPreviousCoord,y,mPenLaserOff);
                mCurrPosY = y;
                mCurrPosX = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case Y:
            writeXAndLaserOn(mCurrPosX);
            if (mCurrPosY != y)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,mCurrPosX,y,mPenLaserOff);
                mCurrPosY = y;
            }
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeXYAndLaserOn(int x, int y)
{
    if (x != mCurrPosX || y != mCurrPosY)
    {
        mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,y,mPenLaserOn);
        mCurrPosY = y;
        mCurrPosX = x;
    }
    return true;
}

bool GraphicsOutputDevice::writeXYAndLaserOff(int x, int y)
{
    if (x != mCurrPosX || y != mCurrPosY)
    {
        if (mDrawIdleMotion)
            mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,y,mPenLaserOff);
        mCurrPosY = y;
        mCurrPosX = x;
    }
    return true;
}

bool GraphicsOutputDevice::writeXAndLaserOn(int x)
{
    if (mRenderMode == STEPS)
    {
        if (x != mCurrPosX)
        {
            mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOn);
            mCurrPosX = x;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            mPreviousCoordType = X;
            mPreviousCoord = x;
            mPreviousLaserState = ON;
            break;
        case Y:
            if (mCurrPosX != x || mCurrPosY != mPreviousCoord)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mPreviousCoord,mPenLaserOn);
                mCurrPosX = x;
                mCurrPosY = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case X:
            writeYAndLaserOn(mCurrPosY);
            writeXAndLaserOn(x);
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeXAndLaserOnDelay(int x)
{
    if (mRenderMode == STEPS)
    {
        if (x != mCurrPosX)
        {
            mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOn);
            mCurrPosX = x;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            if (x != mCurrPosX)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOn);
                mCurrPosX = x;
            }
            break;
        case Y:
            if (mCurrPosX != x || mCurrPosY != mPreviousCoord)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mPreviousCoord,mPenLaserOn);
                mCurrPosX = x;
                mCurrPosY = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case X:
            writeYAndLaserOn(mCurrPosY);
            if (x != mCurrPosX)
            {
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOn);
                mCurrPosX = x;
            }
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeXAndLaserOff(int x)
{
    if (mRenderMode == STEPS)
    {
        if (x != mCurrPosX)
        {
            if (mDrawIdleMotion)
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOff);
            mCurrPosX = x;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            mPreviousCoordType = X;
            mPreviousCoord = x;
            mPreviousLaserState = OFF;
            break;
        case Y:
            if (mCurrPosX != x || mCurrPosY != mPreviousCoord)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mPreviousCoord,mPenLaserOff);
                mCurrPosX = x;
                mCurrPosY = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case X:
            writeYAndLaserOff(mCurrPosY);
            writeXAndLaserOff(x);
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::writeXAndLaserOffDelay(int x)
{
    if (mRenderMode == STEPS)
    {
        if (x != mCurrPosX)
        {
            if (mDrawIdleMotion)
                mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOff);
            mCurrPosX = x;
        }
    } else
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            if (x != mCurrPosX)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOff);
                mCurrPosX = x;
            }
            break;
        case Y:
            if (mCurrPosX != x || mCurrPosY != mPreviousCoord)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mPreviousCoord,mPenLaserOff);
                mCurrPosX = x;
                mCurrPosY = mPreviousCoord;
            }
            mPreviousCoordType = NONE;
            break;
        case X:
            writeYAndLaserOn(mCurrPosY);
            if (x != mCurrPosX)
            {
                if (mDrawIdleMotion)
                    mGraphicsScene->addLine(mCurrPosX,mCurrPosY,x,mCurrPosY,mPenLaserOff);
                mCurrPosX = x;
            }
            break;
        }
    }
    return true;
}

bool GraphicsOutputDevice::begin(MarkDelays *delays)
{
    mGraphics->setScene(0);
    mGraphicsScene->clear();
    if (mPainterPathLaserOn)
        delete mPainterPathLaserOn;
    mPainterPathLaserOn = new QPainterPath();
    if (mPainterPathLaserOff)
        delete mPainterPathLaserOff;
    mPainterPathLaserOff = new QPainterPath();

    mPainterPathLaserOn->moveTo(START_POS_X, START_POS_Y);
    mPainterPathLaserOff->moveTo(START_POS_X, START_POS_Y);

    mCurrPosX = START_POS_X;
    mCurrPosY = START_POS_Y;

    mPreviousCoordType = NONE;

    return true;
}

bool GraphicsOutputDevice::end()
{
    if (mRenderMode == LINES)
    {
        switch (mPreviousCoordType)
        {
        case NONE:
            break;
        case X:
            if (mPreviousLaserState == OFF)
                writeYAndLaserOff(mCurrPosY);
            else
                writeYAndLaserOn(mCurrPosY);
            break;
        case Y:
            if (mPreviousLaserState == OFF)
                writeXAndLaserOff(mCurrPosX);
            else
                writeXAndLaserOn(mCurrPosX);
            break;
        }
    }

    if (mPainterPathLaserOn->elementCount())
    {
        if (mDrawIdleMotion)
            mGraphicsScene->addPath(*mPainterPathLaserOff, mPenLaserOff);
        mGraphicsScene->addPath(*mPainterPathLaserOn, mPenLaserOn);
    }

    // Draw default elements on the scene
    drawOutfieldElements();
    mGraphics->setScene(mGraphicsScene);
    //mGraphics->update();

    return true;
}

bool GraphicsOutputDevice::flush()
{
    return true;
}

void GraphicsOutputDevice::interrupt()
{

}

bool GraphicsOutputDevice::writeLaserOffDelay(int delay)
{
    return true;
}

bool GraphicsOutputDevice::writeLaserOnDelay(int delay)
{
    return true;
}

bool GraphicsOutputDevice::writePermDelay(int delay)
{
    return true;
}

bool GraphicsOutputDevice::writeOnceDelay(int delay)
{
    return true;
}

bool GraphicsOutputDevice::writeQSWPeriod(int delay)
{
    return true;
}

bool GraphicsOutputDevice::writeQSWDuration(int delay)
{
    return true;
}

// functions for fast drawing
bool GraphicsOutputDevice::moveTo(int x, int y)
{
    mPainterPathLaserOn->moveTo(x, y);
    if (mDrawIdleMotion)
        mPainterPathLaserOff->lineTo(x, y);
    return true;
}

bool GraphicsOutputDevice::lineTo(int x, int y)
{
    mPainterPathLaserOn->lineTo(x, y);
    if (mDrawIdleMotion)
        mPainterPathLaserOff->moveTo(x, y);
    return true;
}

bool GraphicsOutputDevice::conicTo(int controlX, int controlY,
                                   int toX, int toY)
{
    mPainterPathLaserOn->quadTo(controlX, controlY, toX, toY);
    if (mDrawIdleMotion)
        mPainterPathLaserOff->moveTo(toX, toY);
    return true;
}

bool GraphicsOutputDevice::cubicTo(int control1X, int control1Y,
                                   int control2X, int control2Y,
                                   int toX, int toY)
{
    mPainterPathLaserOn->cubicTo(control1X, control1Y,
                          control2X, control2Y,
                          toX, toY);
    if (mDrawIdleMotion)
        mPainterPathLaserOff->moveTo(toX, toY);
    return true;
}

bool GraphicsOutputDevice::segmentTo(int centerX, int centerY, int toX, int toY)
{
    int currx = mPainterPathLaserOn->currentPosition().x();
    int curry = mPainterPathLaserOn->currentPosition().y();

    double radius = sqrt((static_cast<double>(currx) - centerX)*(currx - centerX) +
                         (curry - centerY)*(curry - centerY));
    double startAngle = asin((curry - centerY)/radius);
    if (startAngle >= 0)
    {
        if (currx - centerX < 0)
            startAngle = PI - startAngle;
    } else
    {
        if (currx - centerX >= 0)
            startAngle = 2*PI + startAngle;
        else
            startAngle = PI - startAngle;
    }

    double endAngle = asin((toY - centerY)/radius);
    if (endAngle >= 0)
    {
        if (toX - centerX < 0)
            endAngle = PI - endAngle;
    } else
    {
        if (toX - centerX >= 0)
            endAngle = 2*PI + endAngle;
        else
            endAngle = PI - endAngle;
    }

    startAngle = -startAngle*180 / PI;
    endAngle = -endAngle*180 / PI;

    int x = static_cast<int>(round(centerX - radius));
    int y = static_cast<int>(round(centerY - radius));
    int width = radius*2;
    int height = width;

    mPainterPathLaserOn->arcTo(x, y, width, height, startAngle, endAngle - startAngle);
    if (mPainterPathLaserOn->currentPosition().x() != toX ||
        mPainterPathLaserOn->currentPosition().y() != toY)
        mPainterPathLaserOn->lineTo(toX, toY);

    return true;
}
