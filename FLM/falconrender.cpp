#include "stdheader.h"
#include "falconrender.h"

#include <QFile>
#include <QDebug>

// forward functions declaration
int move_to(const FT_Vector*  to, void* user);
int line_to( const FT_Vector*  to, void* user );
int conic_to( const FT_Vector*  control, const FT_Vector*  to, void* user );
int cubic_to( const FT_Vector*  control1, const FT_Vector*  control2,
                             const FT_Vector*  to, void* user );
void raster(const int y, const int count,
            const FT_Span * const spans, void * const user);

FalconRender::FalconRender()
{
    mOutDevice = 0;

    // Initialize callbacks for glyph decomposition
    mFTOutlineCallbacks.move_to = &move_to;
    mFTOutlineCallbacks.line_to = &line_to;
    mFTOutlineCallbacks.conic_to = &conic_to;
    mFTOutlineCallbacks.cubic_to = &cubic_to;
    mFTOutlineCallbacks.shift = 0;
    mFTOutlineCallbacks.delta = 0;

    // Initialize freetype library
    FT_Init_FreeType(&mFTLibrary);

    mCurPosX = START_POS_X;
    mCurPosY = START_POS_Y;
    mLastMoveToX = mLastMoveToY = -1;
    mPrevPointIsOut = false;
    mRenderResultInfo = RENDER_OK;

//    mClosedGlyphs.insert(L'0');
//    mClosedGlyphs.insert(L'8');
//    mClosedGlyphs.insert(L'B');
}

FalconRender::~FalconRender()
{
    FT_Done_FreeType(mFTLibrary);

    for (QMap<QString, MemoryFont*>::iterator iter = mFonts.begin();iter != mFonts.end();iter++)
    {
        MemoryFont* mf = iter.value();
        delete [] mf->mData;
        delete mf;
    }
}

bool FalconRender::begin(OutputDevice* outDevice, FLMSettings *settings, QMatrix* corrMatrix,
                         bool fastDraw)
{
    mFLMSettings = settings;
    mRenderConfig = settings->getActiveConfig();
    mOutDevice = outDevice;
    mCorrMatrix = corrMatrix;
    mFastDraw = fastDraw;

    mRenderResultInfo = RENDER_OK;

    if (!mOutDevice->begin(&mRenderConfig->mDelays))
    {
        mRenderResultInfo = RENDER_ERROR;
        return false;
    }

    if (!mOutDevice->writeXAndLaserOff(START_POS_X))
    {
        mRenderResultInfo = RENDER_ERROR;
        return false;
    }

    if (!mOutDevice->writeYAndLaserOff(START_POS_Y))
    {
        mRenderResultInfo = RENDER_ERROR;
        return false;
    }

    mCurPosX = START_POS_X;
    mCurPosY = START_POS_Y;

    return true;
}

bool FalconRender::end()
{
    bool result = moveTo(START_POS_X, START_POS_Y);
    mLastMoveToX = mLastMoveToY = -1;
    mCorrMatrix = 0;
    if (!mOutDevice->end())
    {
        mRenderResultInfo = RENDER_ERROR;
        result = false;
    }
    return result;
}

ValidatePointIsOutResult FalconRender::validatePointIsOut(int x, int y)
{
    bool pointOut = (x >= FIELD_WIDTH || x < 0 || y >= FIELD_HEIGHT || y < 0);
    if (!pointOut)
    {
        if (mPrevPointIsOut)
        {
            mPrevPointIsOut = false;
            return POINT_IN_CONNECT;
        }
        mLastInPointX = x;
        mLastInPointY = y;
        return POINT_IN;
    } else
    {
        mRenderResultInfo = RENDER_OK_OUT_FIELD;
        mPrevPointIsOut = true;
        return POINT_OUT;
    }
}

bool FalconRender::moveTo(int x, int y)
{
    //qDebug()<<"FalconRender::moveTo("<<x<<","<<y<<")";
    if (mFastDraw)
    {
        if (mCorrMatrix)
            mCorrMatrix->map(x, y, &x, &y);
        return mOutDevice->moveTo(x,y);
    }

    if (mCurPosX == x && mCurPosY == y)
        return true;

    double distance = sqrt((static_cast<double>(x) - mCurPosX)*(x - mCurPosX) +
                           (static_cast<double>(y) - mCurPosY)*(y - mCurPosY));
    double pieces = distance / mRenderConfig->mDelays.mDXY_LaserOff;

    double dx = (x - mCurPosX) / pieces;
    double dy = (y - mCurPosY) / pieces;

    double fCurrX = mCurPosX;
    double fCurrY = mCurPosY;

    int ix = 0x7FFFFFFF, iy = 0x7FFFFFFF;
    ValidatePointIsOutResult pointValidationResult;

    for (int i = 1; i < pieces; i++)
    {
        if (dx != 0)
            fCurrX += dx;

        if (dy != 0)
            fCurrY += dy;

        ix = static_cast<int>(round(fCurrX));
        iy = static_cast<int>(round(fCurrY));

        pointValidationResult = validatePointIsOut(ix, iy);
        if (pointValidationResult == POINT_IN_CONNECT)
        {
            mCurPosX = mLastInPointX;
            mCurPosY = mLastInPointY;
            if (!moveTo(ix, iy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }

        if (ix != mCurPosX || iy != mCurPosY)
        {
            if (pointValidationResult == POINT_IN)
            {
                int cx = ix;
                int cy = iy;
                correctPoint(&cx, &cy);
                if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                    qDebug() << "cx: " << cx << "cy: " << cy << endl;

                if (mCorrMatrix)
                    mCorrMatrix->map(cx, cy, &cx, &cy);

                if (!mOutDevice->writeXAndLaserOff(cx))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
                if (!mOutDevice->writeYAndLaserOffDelay(cy))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
            }
            mCurPosX = ix;
            mCurPosY = iy;
        }
    }

    pointValidationResult = validatePointIsOut(x, y);
    if (pointValidationResult == POINT_IN_CONNECT)
    {
        mCurPosX = mLastInPointX;
        mCurPosY = mLastInPointY;
        if (moveTo(x, y) == RENDER_ERROR)
        {
            mRenderResultInfo = RENDER_ERROR;
            return false;
        }
    }

    if (mCurPosX != x || mCurPosY != y)
    {
        if (pointValidationResult == POINT_IN)
        {
            int cx = x;
            int cy = y;
            correctPoint(&cx, &cy);
            if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                qDebug() << "cx: " << cx << "cy: " << cy << endl;

            if (mCorrMatrix)
                mCorrMatrix->map(cx, cy, &cx, &cy);

            if (!mOutDevice->writeXAndLaserOff(cx))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
            if (!mOutDevice->writeYAndLaserOffDelay(cy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }
        mCurPosX = x;
        mCurPosY = y;
    }

    return true;
}

bool FalconRender::lineTo(int x, int y)
{
    //qDebug()<<"FalconRender::lineTo("<<x<<","<<y<<")";
    if (mFastDraw)
    {
        if (mCorrMatrix)
            mCorrMatrix->map(x, y, &x, &y);
        return mOutDevice->lineTo(x, y);
    }

    if (mCurPosX == x && mCurPosY == y)
        return true;

    double distance = sqrt((static_cast<double>(x) - mCurPosX)*(x - mCurPosX) +
                           (static_cast<double>(y) - mCurPosY)*(y - mCurPosY));
    double pieces = distance / mRenderConfig->mDelays.mDXY_LaserOn;

    double dx = (x - mCurPosX) / pieces;
    double dy = (y - mCurPosY) / pieces;

    double fCurrX = mCurPosX;
    double fCurrY = mCurPosY;

    int ix = 0x7FFFFFFF, iy = 0x7FFFFFFF;
    ValidatePointIsOutResult pointValidationResult;

    for (int i = 1; i < pieces; i++)
    {
        if (dx != 0)
            fCurrX += dx;

        if (dy != 0)
            fCurrY += dy;

        ix = static_cast<int>(round(fCurrX));
        iy = static_cast<int>(round(fCurrY));

        pointValidationResult = validatePointIsOut(ix, iy);
        if (pointValidationResult == POINT_IN_CONNECT)
        {
            mCurPosX = mLastInPointX;
            mCurPosY = mLastInPointY;
            if (moveTo(ix, iy) == RENDER_ERROR)
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }

        if (ix != mCurPosX || iy != mCurPosY)
        {
            if (pointValidationResult == POINT_IN)
            {
                int cx = ix;
                int cy = iy;
                correctPoint(&cx, &cy);
                if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                    qDebug() << "cx: " << cx << "cy: " << cy << endl;

                if (mCorrMatrix)
                    mCorrMatrix->map(cx, cy, &cx, &cy);

                if (!mOutDevice->writeXAndLaserOn(cx))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
                if (!mOutDevice->writeYAndLaserOnDelay(cy))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
            }
            mCurPosX = ix;
            mCurPosY = iy;
        }
    }

    pointValidationResult = validatePointIsOut(x, y);
    if (pointValidationResult == POINT_IN_CONNECT)
    {
        mCurPosX = mLastInPointX;
        mCurPosY = mLastInPointY;
        if (moveTo(x, y) == RENDER_ERROR)
        {
            mRenderResultInfo = RENDER_ERROR;
            return false;
        }
    }

    if (mCurPosX != x || mCurPosY != y)
    {
        if (pointValidationResult == POINT_IN)
        {
            int cx = x;
            int cy = y;
            correctPoint(&cx, &cy);
            if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                qDebug() << "cx: " << cx << "cy: " << cy << endl;

            if (mCorrMatrix)
                mCorrMatrix->map(cx, cy, &cx, &cy);

            if (!mOutDevice->writeXAndLaserOn(cx))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
            if (!mOutDevice->writeYAndLaserOnDelay(cy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }
        mCurPosX = x;
        mCurPosY = y;
    }
    return true;
}

double FalconRender::getConicLength(int fromX,
                                    int fromY,
                                    int controlX,
                                    int controlY,
                                    int toX,
                                    int toY)
{
    double beizer_x;
    double beizer_y;
    double prev_beizer_x = fromX;
    double prev_beizer_y = fromY;
    double length = 0.0;
    double pointsDistance = sqrt((toX - fromX)*(toX - fromX) + (toY - fromY)*(toY - fromY));
    double dt = 1/pointsDistance;
    double t = dt;

    while (t <= 1)
    {
        beizer_x = (1-t)*(1-t)*fromX + 2*t*(1-t)*controlX+t*t*toX;
        beizer_y = (1-t)*(1-t)*fromY + 2*t*(1-t)*controlY+t*t*toY;
        length += sqrt((beizer_x - prev_beizer_x)*(beizer_x - prev_beizer_x) +
                       (beizer_y - prev_beizer_y)*(beizer_y - prev_beizer_y));
        prev_beizer_x = beizer_x;
        prev_beizer_y = beizer_y;
        t += dt;
    }

    length += sqrt((toX - prev_beizer_x)*(toX - prev_beizer_x) +
                   (toY - prev_beizer_y)*(toY - prev_beizer_y));

    return length;
}

// Second order Beizer arc
// polinomial P(t) = (1-t)^2*P0 + 2t*(1 - t)*P1 + t^2*P2
bool FalconRender::conicTo(int controlX,
                           int controlY,
                           int toX,
                           int toY)
{
    //qDebug()<<"FalconRender::conicTo("<<controlX<<","<<controlY<<","<<toX<<","<<toY<<")";
    if (mFastDraw)
    {
        if (mCorrMatrix)
        {
            mCorrMatrix->map(controlX, controlY, &controlX, &controlY);
            mCorrMatrix->map(toX, toY, &toX, &toY);
        }
        return mOutDevice->conicTo(controlX, controlY, toX, toY);
    }

    if (mCurPosX == toX && mCurPosY == toY)
        return true;

    double distance = getConicLength(mCurPosX,mCurPosY,controlX,controlY,toX,toY);
    double pieces = distance / mRenderConfig->mDelays.mDXY_LaserOn;

    double dt = 1 / pieces;
    double t = dt;
    int beizer_x = -1;
    int beizer_y = -1;
    ValidatePointIsOutResult pointValidationResult;
    int startX = mCurPosX;
    int startY = mCurPosY;

    while (t < 1)
    {
        beizer_x = round((1-t)*(1-t)*startX + 2*t*(1-t)*controlX+t*t*toX);
        beizer_y = round((1-t)*(1-t)*startY + 2*t*(1-t)*controlY+t*t*toY);

        pointValidationResult = validatePointIsOut(beizer_x, beizer_y);
        if (pointValidationResult == POINT_IN_CONNECT)
        {
            mCurPosX = mLastInPointX;
            mCurPosY = mLastInPointY;
            if (!moveTo(beizer_x, beizer_y))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }

        if (beizer_x != mCurPosX || beizer_y != mCurPosY)
        {
            if (pointValidationResult == POINT_IN)
            {
                int cx = beizer_x;
                int cy = beizer_y;
                correctPoint(&cx, &cy);
                if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                    qDebug() << "cx: " << cx << "cy: " << cy << endl;

                if (mCorrMatrix)
                    mCorrMatrix->map(cx, cy, &cx, &cy);

                if (!mOutDevice->writeXAndLaserOn(cx))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
                if (!mOutDevice->writeYAndLaserOnDelay(cy))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
            }
            mCurPosX = beizer_x;
            mCurPosY = beizer_y;
        }
        t += dt;
    }

    pointValidationResult = validatePointIsOut(toX, toY);
    if (pointValidationResult == POINT_IN_CONNECT)
    {
        mCurPosX = mLastInPointX;
        mCurPosY = mLastInPointY;
        if (!moveTo(toX, toY))
        {
            mRenderResultInfo = RENDER_ERROR;
            return false;
        }
    }

    if (mCurPosX != toX || mCurPosY != toY)
    {
        if (pointValidationResult == POINT_IN)
        {
            int cx = toX;
            int cy = toY;
            correctPoint(&cx, &cy);
            if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                qDebug() << "cx: " << cx << "cy: " << cy << endl;

            if (mCorrMatrix)
                mCorrMatrix->map(cx, cy, &cx, &cy);

            if (!mOutDevice->writeXAndLaserOn(cx))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
            if (!mOutDevice->writeYAndLaserOnDelay(cy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }
        mCurPosX = toX;
        mCurPosY = toY;
    }
    return true;
}

double FalconRender::getCubicLength(int fromX,
                                   int fromY,
                                   int control1X,
                                   int control1Y,
                                   int control2X,
                                   int control2Y,
                                   int toX,
                                   int toY)
{
    double beizer_x;
    double beizer_y;
    double prev_beizer_x = fromX;
    double prev_beizer_y = fromY;
    double length = 0.0;
    double pointsDistance = sqrt((toX - fromX)*(toX - fromX) + (toY - fromY)*(toY - fromY));
    double dt = 1/pointsDistance;
    double t = dt;

    while (t <= 1)
    {
        beizer_x = (1-t)*(1-t)*(1-t)*fromX + 3*t*(1-t)*(1-t)*control1X +
                   3*t*t*(1-t)*control2X + t*t*t*toX;
        beizer_y = (1-t)*(1-t)*(1-t)*fromY + 3*t*(1-t)*(1-t)*control1Y +
                   3*t*t*(1-t)*control2Y + t*t*t*toY;
        length += sqrt((beizer_x - prev_beizer_x)*(beizer_x - prev_beizer_x) +
                       (beizer_y - prev_beizer_y)*(beizer_y - prev_beizer_y));
        prev_beizer_x = beizer_x;
        prev_beizer_y = beizer_y;
        t += dt;
    }

    length += sqrt((toX - prev_beizer_x)*(toX - prev_beizer_x) +
                   (toY - prev_beizer_y)*(toY - prev_beizer_y));

    return length;
}

// Third order Beizer arc
// polynomial P(t) = (1-t)^3*P1 + 3*t*(1-t)^2*P2 + 3*t^2*(1-t)*P3 + t^3*P4
bool FalconRender::cubicTo(int control1X,
                           int control1Y,
                           int control2X,
                           int control2Y,
                           int toX,
                           int toY)
{
    //qDebug()<<"FalconRender::cubicTo("<<control1X<<","<<control1Y<<","<<
    //        control2X<<","<<control2Y<<","<<toX<<","<<toY<<")";
    if (mFastDraw)
    {
        if (mCorrMatrix)
        {
            mCorrMatrix->map(control1X, control1Y, &control1X, &control1Y);
            mCorrMatrix->map(control2X, control2Y, &control2X, &control2Y);
            mCorrMatrix->map(toX, toY, &toX, &toY);
        }

        return mOutDevice->cubicTo(control1X, control1Y,
                                   control2X, control2Y,
                                   toX, toY);
    }

    if (mCurPosX == toX && mCurPosY == toY)
        return true;

    double distance = getCubicLength(mCurPosX, mCurPosY,
                                    control1X, control1Y,
                                    control2X, control2Y,
                                    toX,toY);

    double pieces = distance / mRenderConfig->mDelays.mDXY_LaserOn;

    double dt = 1 / pieces;
    double t = dt;
    int beizer_x = -1;
    int beizer_y = -1;
    ValidatePointIsOutResult pointValidationResult;
    int startX = mCurPosX;
    int startY = mCurPosY;

    while (t < 1)
    {
        beizer_x = round((1-t)*(1-t)*(1-t)*startX + 3*t*(1-t)*(1-t)*control1X +
                   3*t*t*(1-t)*control2X + t*t*t*toX);
        beizer_y = round((1-t)*(1-t)*(1-t)*startY + 3*t*(1-t)*(1-t)*control1Y +
                   3*t*t*(1-t)*control2Y + t*t*t*toY);

        pointValidationResult = validatePointIsOut(beizer_x, beizer_y);
        if (pointValidationResult == POINT_IN_CONNECT)
        {
            mCurPosX = mLastInPointX;
            mCurPosY = mLastInPointY;
            if (!moveTo(beizer_x, beizer_y))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }

        if (beizer_x != mCurPosX || beizer_y != mCurPosY)
        {
            if (pointValidationResult == POINT_IN)
            {
                int cx = beizer_x;
                int cy = beizer_y;
                correctPoint(&cx, &cy);
                if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                    qDebug() << "cx: " << cx << "cy: " << cy << endl;

                if (mCorrMatrix)
                    mCorrMatrix->map(cx, cy, &cx, &cy);

                if (!mOutDevice->writeXAndLaserOn(cx))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
                if (!mOutDevice->writeYAndLaserOnDelay(cy))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
            }
            mCurPosX = beizer_x;
            mCurPosY = beizer_y;
        }
        t += dt;
    }

    pointValidationResult = validatePointIsOut(toX, toY);
    if (pointValidationResult == POINT_IN_CONNECT)
    {
        mCurPosX = mLastInPointX;
        mCurPosY = mLastInPointY;
        if (!moveTo(toX, toY))
        {
            mRenderResultInfo = RENDER_ERROR;
            return false;
        }
    }

    if (mCurPosX != toX || mCurPosY != toY)
    {
        if (pointValidationResult == POINT_IN)
        {
            int cx = toX;
            int cy = toY;
            correctPoint(&cx, &cy);
            if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                qDebug() << "cx: " << cx << "cy: " << cy << endl;

            if (mCorrMatrix)
                mCorrMatrix->map(cx, cy, &cx, &cy);

            if (!mOutDevice->writeXAndLaserOn(cx))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
            if (!mOutDevice->writeYAndLaserOnDelay(cy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }
        mCurPosX = toX;
        mCurPosY = toY;
    }
    return true;
}

bool FalconRender::segmentTo(int centerX, int centerY, int toX, int toY)
{
    if (mCorrMatrix != 0)
    {
        mCorrMatrix->map(centerX, centerY, &centerX, &centerY);
        mCorrMatrix->map(toX, toY, &toX, &toY);
    }

    if (mFastDraw)
        return mOutDevice->segmentTo(centerX, centerY, toX, toY);

    if (mCurPosX == toX && mCurPosY == toY)
        return true;

    double radius = sqrt((static_cast<double>(mCurPosX) - centerX)*(mCurPosX - centerX) +
                         (mCurPosY - centerY)*(mCurPosY - centerY));
    double startAngle = asin((mCurPosY - centerY)/radius);
    if (startAngle >= 0)
    {
        if (mCurPosX - centerX < 0)
            startAngle = PI - startAngle;
    } else
    {
        if (mCurPosX - centerX >= 0)
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

    double distance = (endAngle - startAngle)*radius;
    if (distance < 0)
        distance = -distance;
    double pieces = distance / mRenderConfig->mDelays.mDXY_LaserOn;

    double dalpha = (endAngle - startAngle) / pieces;
    double fCurrAlpha = startAngle;

    int ix = 0x7FFFFFFF, iy = 0x7FFFFFFF;
    ValidatePointIsOutResult pointValidationResult;

    for (int i = 1; i < pieces; i++)
    {
        fCurrAlpha += dalpha;
        ix = static_cast<int>(round(radius*cos(fCurrAlpha) + centerX));
        iy = static_cast<int>(round(radius*sin(fCurrAlpha) + centerY));

        pointValidationResult = validatePointIsOut(ix, iy);
        if (pointValidationResult == POINT_IN_CONNECT)
        {
            mCurPosX = mLastInPointX;
            mCurPosY = mLastInPointY;
            if (moveTo(ix, iy) == RENDER_ERROR)
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }

        if (ix != mCurPosX || iy != mCurPosY)
        {
            if (pointValidationResult == POINT_IN)
            {
                int cx = ix;
                int cy = iy;
                correctPoint(&cx, &cy);
                if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                    qDebug() << "cx: " << cx << "cy: " << cy << endl;

                if (mCorrMatrix)
                    mCorrMatrix->map(cx, cy, &cx, &cy);

                if (!mOutDevice->writeXAndLaserOn(cx))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
                if (!mOutDevice->writeYAndLaserOnDelay(cy))
                {
                    mRenderResultInfo = RENDER_ERROR;
                    return false;
                }
            }
            mCurPosX = ix;
            mCurPosY = iy;
        }
    }

    pointValidationResult = validatePointIsOut(toX, toY);
    if (pointValidationResult == POINT_IN_CONNECT)
    {
        mCurPosX = mLastInPointX;
        mCurPosY = mLastInPointY;
        if (moveTo(toX, toY) == RENDER_ERROR)
        {
            mRenderResultInfo = RENDER_ERROR;
            return false;
        }
    }

    if (mCurPosX != toX || mCurPosY != toY)
    {
        if (pointValidationResult == POINT_IN)
        {
            int cx = toX;
            int cy = toY;
            correctPoint(&cx, &cy);
            if (cx < 0 || cx > 65536 || cy < 0 || cy > 65535)
                qDebug() << "cx: " << cx << "cy: " << cy << endl;

            if (mCorrMatrix)
                mCorrMatrix->map(cx, cy, &cx, &cy);

            if (!mOutDevice->writeXAndLaserOn(cx))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
            if (!mOutDevice->writeYAndLaserOnDelay(cy))
            {
                mRenderResultInfo = RENDER_ERROR;
                return false;
            }
        }
        mCurPosX = toX;
        mCurPosY = toY;
    }
    return true;
}

void FalconRender::correctPoint(int *xin, int *yin)
{
    if (!mFLMSettings->getActiveConfig()->mCorrTurnedOn || !mFLMSettings->mCorrMatrixLoaded)
        return;

    double kXp, kYp, Px1, Py1;
    double P, Px, Py, D, B, R;
    int kx00, ky00, kx01, ky01, kx10, ky10, kx11, ky11;
    int XX0, YY0, XX1, YY1;

    const int h = 1024;

    int xp = *xin;
    int yp = *yin;

    kXp = 0;kYp = 0;
    XX0 = static_cast<int>(xp/static_cast<double>(h));
    YY0 = static_cast<int>(yp/static_cast<double>(h));
    XX1 = XX0 + 1;YY1 = YY0 + 1;
    kx00 = mFLMSettings->mCorrMatrix[XX0][YY0][0];// kx00,ky00 --> X0,Y0;
    ky00 = mFLMSettings->mCorrMatrix[XX0][YY0][1];
    kx01 = mFLMSettings->mCorrMatrix[XX1][YY0][0];// kx01,ky01 --> X1,Y0;
    ky01 = mFLMSettings->mCorrMatrix[XX1][YY0][1];
    kx10 = mFLMSettings->mCorrMatrix[XX0][YY1][0];// kx10,ky10 --> X0,Y1;
    ky10 = mFLMSettings->mCorrMatrix[XX0][YY1][1];
    kx11 = mFLMSettings->mCorrMatrix[XX1][YY1][0];// kx11,ky11 --> X1,Y1;
    ky11 = mFLMSettings->mCorrMatrix[XX1][YY1][1];

    B = XX0*h;
    R = xp-B;P = mFLMSettings->mCx[XX0][YY0];D = mFLMSettings->mCx[XX1][YY0];
    B = (kx01-kx00)/h - (D+2*P)*h/3;D = (D-P)/h*R;
    Px = kx00+R*(B+R*(P+D/3));
    //??????? ???????? ??????? ??? ?????? ??????? [XX0,YY0; XX0,YY1]
    B = YY0*h;
    R = yp-B;P = mFLMSettings->mCy[XX0][YY0];D = mFLMSettings->mCy[XX0][YY1];
    B = (ky10-ky00)/h - (D+2*P)*h/3;D = (D-P)/h*R;
    Py = ky00+R*(B+R*(P+D/3));
    //??????? ???????? ??????? ??? ???????? ??????? [XX0,YY1; XX1,YY1]
    B = XX0*h;
    R = xp-B;P = mFLMSettings->mCx[XX0][YY1];D = mFLMSettings->mCx[XX1][YY1];
    B = (kx11-kx10)/h - (D+2*P)*h/3; D = (D-P)/h*R;
    Px1 = kx10+R*(B+R*(P+D/3));
    //??????? ???????? ??????? ??? ??????? ??????? [XX1,YY0; XX1,YY1]
    B = YY0*h;
    R = yp-B;P = mFLMSettings->mCy[XX1][YY0];D = mFLMSettings->mCy[XX1][YY1];
    B = (ky11-ky01)/h - (D+2*P)*h/3;D = (D-P)/h*R;
    Py1 = ky01+R*(B+R*(P+D/3));

    *xin = static_cast<int>(round(xp + Px + ((Px1-Px) * ((yp-YY0*h)/h))));
    *yin = static_cast<int>(round(yp + Py + ((Py1-Py) * ((xp-XX0*h)/h))));
}

void FalconRender::onRaster(const int y, const int count,
                          const FT_Span * const spans)
{
    if (y % mPrintEveryLine == 0)
    {
        if (!mSpans->contains(y))
            mSpans->insert(y, new pointsvector());

        if (mLastY != y)
        {
            mLastY = y;
            mSpans->value(y)->push_back(QPoint(spans->x + mCurrCharDelta.x / 64.0,
                                               y + mCurrCharDelta.y / 64.0));
            mSpans->value(y)->push_back(QPoint(spans->x + spans->len + mCurrCharDelta.x / 64.0,
                                               y + mCurrCharDelta.y / 64.0));
        }

        const FT_Span* currSpan;
        for (int i = 1;i < count;i++)
        {
            currSpan = spans + i;

            if (mSpans->value(y)->last().x() == currSpan->x + mCurrCharDelta.x / 64.0)
                mSpans->value(y)->last().setX(currSpan->x + currSpan->len + mCurrCharDelta.x / 64.0);
            else
            {
                mSpans->value(y)->push_back(QPoint(currSpan->x + mCurrCharDelta.x / 64.0,
                                                   y + mCurrCharDelta.y / 64.0));
                mSpans->value(y)->push_back(QPoint(currSpan->x + currSpan->len + mCurrCharDelta.x / 64.0,
                                                   y + mCurrCharDelta.y / 64.0));
            }
        }
    }
}

void FalconRender::allocateSpans()
{
    mSpans = new spansmap();
}

bool FalconRender::rasterSpans()
{
    spansmap::const_iterator mapIterator = mSpans->constBegin();
    int currVectorNum = 0;
    while (mapIterator != mSpans->constEnd())
    {
        pointsvector* currVector = mapIterator.value();
        if (currVectorNum % 2 == 0)
        {
            for (int point = 0;point < currVector->size();point += 2)
            {
                if (!moveTo(currVector->at(point).x(), currVector->at(point).y()))
                    return false;
                if (!lineTo(currVector->at(point + 1).x(), currVector->at(point + 1).y()))
                    return false;
            }
        } else
        {
            for (int point = currVector->size() - 1;point >= 0;point -= 2)
            {
                if (!moveTo(currVector->at(point).x(), currVector->at(point).y()))
                    return false;
                if (!lineTo(currVector->at(point - 1).x(), currVector->at(point - 1).y()))
                    return false;
            }
        }
        ++mapIterator;
        ++currVectorNum;
    }

    return true;
}

void FalconRender::releaseSpans()
{
    spansmap::const_iterator mapIterator = mSpans->constBegin();
    pointsvector* currVector;

    while (mapIterator != mSpans->constEnd())
    {
        currVector = mapIterator.value();
        delete currVector;
        ++mapIterator;
    }

    delete mSpans;
}

bool FalconRender::getTextMetrics(QString &text, int size, MemoryFont* font,
                                  float scaleX, float scaleY, TextMetrics* metrics)
{
    double width = 0;
    double height = 0;

    FT_Face fontFace;

    if (FT_New_Memory_Face(mFTLibrary, font->mData, font->mSize, 0, &fontFace))
        return false;

    if (FT_Set_Char_Size(fontFace,
                         size*64.0,
                         size*64.0,
                         HORZ_RESOLUTION,
                         VERT_RESOLUTION))
    {
        FT_Done_Face(fontFace);
        return false;
    }

    FT_Matrix matrix;
    memset(&matrix, 0, sizeof(FT_Matrix));
    if (scaleX <= 0) {
        scaleX = 1.0;
        matrix.xx = 1 << 16;
    } else
        matrix.xx = (static_cast<double>(scaleX) * 65536.0 + 0.5);

    if (scaleY <= 0) {
        scaleY = 1.0;
        matrix.yy = 1 << 16;
    } else
        matrix.yy = (static_cast<double>(scaleY) * 65536.0 + 0.5);

    int last = text.length() - 1;
    for (int i = 0;i < text.length();i++)
    {
        FT_UInt gindex = FT_Get_Char_Index(fontFace,
                                           static_cast<FT_ULong>(text.at(i).unicode()));
        FT_Set_Transform(fontFace, &matrix, 0);

        if (!FT_Load_Glyph(fontFace, gindex, FT_LOAD_NO_BITMAP))
        {
            width += fontFace->glyph->metrics.horiAdvance * scaleX;
            height = qMax(height, static_cast<double>(fontFace->glyph->metrics.horiBearingY * scaleY));

            if (i == 0)
                metrics->mSpaceBefore = fontFace->glyph->metrics.horiBearingX;
            if (i == last)
                metrics->mSpaceAfter = fontFace->glyph->metrics.horiAdvance -
                                       fontFace->glyph->metrics.horiBearingX -
                                       fontFace->glyph->metrics.width;
        } else
        {
            FT_Done_Face(fontFace);
            return false;
        }
    }

    metrics->mHeight = height / 64.0;
    metrics->mWidth = width / 64.0;
    metrics->mSpaceBefore /= 64.0;
    metrics->mSpaceAfter /= 64.0;

    FT_Done_Face(fontFace);
    return true;
}

MemoryFont* FalconRender::loadFont(QString &fontPath)
{
    MemoryFont* memFont;

    // check first if font already loaded
    memFont = mFonts.value(fontPath, 0);
    if (!memFont)
    {
        // load font
        FT_Face fontFace;

        QFile fontFile(fontPath);
        if (!fontFile.open(QIODevice::ReadOnly))
        {
            mRenderResultInfo = RENDER_ERROR;
            return 0;
        }

        qint64 size = fontFile.size();

        if (size == 0)
        {
            fontFile.close();
            mRenderResultInfo = RENDER_ERROR;
            return 0;
        }

        FT_Byte* fontBuffer = new FT_Byte[size];
        if (fontFile.read(reinterpret_cast<char*>(fontBuffer),size) != size){
            fontFile.close();
            delete [] fontBuffer;
            mRenderResultInfo = RENDER_ERROR;
            return 0;
        }

        // free resources
        fontFile.close();

        memFont = new MemoryFont();
        memFont->mData = fontBuffer;
        memFont->mSize = static_cast<int>(size);
        // store font for later use
        mFonts[fontPath] = memFont;

        if (FT_New_Memory_Face(mFTLibrary, memFont->mData, memFont->mSize, 0, &fontFace) == 0)
        {
            if (fontFace->family_name == 0)
                memFont->mFLMFont = false;
            else
                memFont->mFLMFont = QString(fontFace->family_name).startsWith(FLM_FONT);
        }

        FT_Done_Face(fontFace);
    }

    return memFont;
}

bool FalconRender::line(ImageObjectLine *line) {
    if (!moveTo(line->mFromX, line->mFromY))
        return false;

    if (!lineTo(line->mToX, line->mToY))
        return false;

    return true;
}

bool FalconRender::picture(ImageObjectPicture *picture) {
    QImage image;
    if (!picture->mCachedImage.isNull())
        image = picture->mCachedImage;
    else {
        if (picture->mActiveSize == SIZE_SCALED) {
            if (picture->mScaleX != 1.0f || picture->mScaleY != 1.0f)
                image = picture->mImage.scaled(picture->mScaleX * picture->mImage.width(),
                                               picture->mScaleY * picture->mImage.height(),
                                               Qt::IgnoreAspectRatio);
            else
                image = picture->mImage;
        } else {
            image = picture->mImage.scaled(picture->mWidth,
                                           picture->mHeight,
                                           Qt::IgnoreAspectRatio);
        }
        picture->mCachedImage = image;
    }

    int height = image.height();
    int width = image.width();
    int halfHeight = round(height / 2.0f);
    int halfWidth = round(width / 2.0f);
    int dir = 0;
    for (int y = 0; y < height; ++y) {
        if (y % picture->mDrawEveryLine == 0) {
            if (dir % 2) {
                int from = -1, to = -1;
                QRgb col;
                int x = width - 1;
                while (x >= 0) {
                    while (x >= 0) {
                        col = image.pixel(x, y);
                        if (qRed(col) > 0 && qGreen(col) > 0 && qBlue(col) > 0)
                            --x;
                        else
                            break;
                    }
                    if (x >= 0)
                        from = x;
                    else
                        break;
                    while (x >= 0) {
                        col = image.pixel(x, y);
                        if (qRed(col) == 0 && qGreen(col) == 0 && qBlue(col) == 0)
                            --x;
                        else
                            break;
                    }
                    if (x < from) {
                        if (x >= 0)
                            to = x + 1;
                        else
                            to = 0;
                    }

                    if (to < from) {
                        moveTo(from + picture->mShiftX - halfWidth, halfHeight - y + picture->mShiftY);
                        lineTo(to + picture->mShiftX - halfWidth, halfHeight - y + picture->mShiftY);
                        from = to = -1;
                    }
                }
            } else {
                int from = -1, to = -1;
                QRgb col;
                int x = 0;
                while (x < width) {
                    while (x < width) {
                        col = image.pixel(x, y);
                        if (qRed(col) > 0 && qGreen(col) > 0 && qBlue(col) > 0)
                            ++x;
                        else
                            break;
                    }
                    if (x < width)
                        from = x;
                    else
                        break;
                    while (x < width) {
                        col = image.pixel(x, y);
                        if (qRed(col) == 0 && qGreen(col) == 0 && qBlue(col) == 0)
                            ++x;
                        else
                            break;
                    }
                    if (x > from) {
                        if (x < width)
                            to = x - 1;
                        else
                            to = width - 1;
                    }

                    if (to > from) {
                        moveTo(from - halfWidth + picture->mShiftX, halfHeight - y + picture->mShiftY);
                        lineTo(to - halfWidth + picture->mShiftX, halfHeight - y + picture->mShiftY);
                        from = to = -1;
                    }
                }
            }
            ++dir;
        }
    }
    return true;
}

bool FalconRender::text(ImageObjectText *iot, TextMetrics *tm)
{
    if (iot->mText.length() <= 0)
        return true;

    FT_Face fontFace;
    bool result = true;

    MemoryFont* memFont = loadFont(iot->mFont);

    if (!memFont)
    {
        mRenderResultInfo = RENDER_ERROR;
        return false;
    }

    TextMetrics textMetrics;

    if (!tm) {
        if (!getTextMetrics(iot->mText, iot->mSize, memFont,
                            iot->mScaleX, iot->mScaleY, &textMetrics))
            return false;
    } else {
        textMetrics.mHeight = tm->mHeight;
        textMetrics.mSpaceAfter = tm->mSpaceAfter;
        textMetrics.mSpaceBefore = tm->mSpaceBefore;
        textMetrics.mWidth = tm->mWidth;
    }

    if (FT_New_Memory_Face(mFTLibrary, memFont->mData, memFont->mSize, 0, &fontFace) == 0)
    {
        if (FT_Set_Char_Size(fontFace,
                             iot->mSize*64.0,
                             iot->mSize*64.0,
                             HORZ_RESOLUTION,
                             VERT_RESOLUTION) == 0)
        {
//            mCurrTextHeight = 0;
            FT_Matrix matrix;
            memset(&matrix, 0, sizeof(FT_Matrix));
            if (iot->mScaleX <= 0) {
                iot->mScaleX = 1.0;
                matrix.xx = 1 << 16;
            } else
                matrix.xx = (static_cast<double>(iot->mScaleX) * 65536.0 + 0.5);

            if (iot->mScaleY <= 0) {
                iot->mScaleY = 1.0;
                matrix.yy = 1 << 16;
            } else
                matrix.yy = (static_cast<double>(iot->mScaleY) * 65536.0 + 0.5);

            if (iot->mRenderTextWay == RENDER_INLINE ||
                iot->mRenderTextWay == RENDER_BOTH)
            {

                double outAngle = 0, curAngle = 0, stepAngle = 0;
                double x, y;
                if (iot->mRadius > 0) {
                    outAngle = (textMetrics.mWidth
                                + (iot->mText.length() - 1) * iot->mAdditionalLetterSpace) / static_cast<double>(iot->mRadius);
                    curAngle = 3*Pi / 2 - outAngle / 2;
                    stepAngle = outAngle / (iot->mText.length() - 1);
                    x = iot->mRadius * cos(curAngle) + iot->mOffsetX;
                    y = iot->mRadius * sin(curAngle) + iot->mOffsetY + iot->mRadius - textMetrics.mHeight / 2.0;
                    mCurrCharDelta.x = x * 64.0;
                    mCurrCharDelta.y = y * 64.0;
                } else {
                    mCurrCharDelta.x = (iot->mOffsetX - (iot->mText.length() - 1) * iot->mAdditionalLetterSpace / 2.0
                                        - textMetrics.mWidth / 2.0 - (textMetrics.mSpaceBefore - textMetrics.mSpaceAfter) / 2.0) * 64.0;
                    mCurrCharDelta.y = (iot->mOffsetY - textMetrics.mHeight / 2.0) * 64.0;
                }

                allocateSpans();

                for (int i = 0;i < iot->mText.length();i++)
                {
                    unsigned short ch = iot->mText.at(i).unicode();

                    // Load the glyph
                    FT_UInt gindex = FT_Get_Char_Index(fontFace,
                                                       static_cast<FT_ULong>(ch));

                    FT_Set_Transform(fontFace, &matrix, 0);

                    if (FT_Load_Glyph(fontFace, gindex, FT_LOAD_NO_BITMAP) == 0)
                    {
                        mPrintEveryLine = iot->mPrintEveryLine;
                        mLastY = -1;

                        FT_Raster_Params params;
                        memset(&params, 0, sizeof(params));
                        params.flags = FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA;
                        params.gray_spans = raster;
                        params.user = this;

                        result &= FT_Outline_Render(mFTLibrary,
                                                   &fontFace->glyph->outline,
                                                   &params) == 0;

                        if (iot->mRadius > 0) {
                            curAngle += stepAngle;
                            x = iot->mRadius * cos(curAngle) + iot->mOffsetX;
                            y = iot->mRadius * sin(curAngle) + iot->mOffsetY + iot->mRadius - textMetrics.mHeight / 2.0;
                            mCurrCharDelta.x = x * 64.0;
                            mCurrCharDelta.y = y * 64.0;
                        }
                        else {
                            mCurrCharDelta.x += fontFace->glyph->metrics.horiAdvance * iot->mScaleX + iot->mAdditionalLetterSpace * 64.0;
                        }
//                        mCurrTextHeight = qMax(mCurrTextHeight, static_cast<int>(fontFace->glyph->metrics.vertAdvance * iot->mScaleY));
                    }
                }

                result &= rasterSpans();
                // delete mSpans and all vectors
                releaseSpans();

            }

            if (iot->mRenderTextWay == RENDER_OUTLINE ||
                iot->mRenderTextWay == RENDER_BOTH)
            {
                double outAngle = 0, curAngle = 0, stepAngle = 0;
                double x, y;
                if (iot->mRadius > 0) {
                    outAngle = (textMetrics.mWidth
                                + (iot->mText.length() - 1) * iot->mAdditionalLetterSpace) / static_cast<double>(iot->mRadius);
                    curAngle = 3*Pi / 2 - outAngle / 2;
                    stepAngle = outAngle / (iot->mText.length() - 1);
                    x = iot->mRadius * cos(curAngle) + iot->mOffsetX;
                    y = iot->mRadius * sin(curAngle) + iot->mOffsetY + iot->mRadius - textMetrics.mHeight / 2.0;
                    mCurrCharDelta.x = x * 64.0;
                    mCurrCharDelta.y = y * 64.0;
                } else {
                    mCurrCharDelta.x = (iot->mOffsetX - (iot->mText.length() - 1) * iot->mAdditionalLetterSpace / 2.0
                                        - textMetrics.mWidth / 2.0 - (textMetrics.mSpaceBefore - textMetrics.mSpaceAfter) / 2.0) * 64.0;
                    mCurrCharDelta.y = (iot->mOffsetY - textMetrics.mHeight / 2.0) * 64.0;
                }

                for (int i = 0;i < iot->mText.length();i++)
                {
                    unsigned short ch = iot->mText.at(i).unicode();

                    // Load the glyph
                    FT_UInt gindex = FT_Get_Char_Index(fontFace,
                                                       static_cast<FT_ULong>(ch));

                    FT_Set_Transform(fontFace, &matrix, &mCurrCharDelta);

                    if (FT_Load_Glyph(fontFace, gindex, FT_LOAD_NO_BITMAP) == 0)
                    {
                        {
                            if (fontFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                            {
                                mIsGlyphClosed = true;
                                if (memFont->mFLMFont)
                                    if (!mClosedGlyphs.contains(iot->mText.at(i).unicode()))
                                        mIsGlyphClosed = false;

                                result &= FT_Outline_Decompose(&fontFace->glyph->outline,
                                                     &mFTOutlineCallbacks, this) == 0;
                            }
                        }
                        if (iot->mRadius > 0) {
                            curAngle += stepAngle;
                            x = iot->mRadius * cos(curAngle) + iot->mOffsetX;
                            y = iot->mRadius * sin(curAngle) + iot->mOffsetY + iot->mRadius - textMetrics.mHeight / 2.0;
                            mCurrCharDelta.x = x * 64.0;
                            mCurrCharDelta.y = y * 64.0;
                        }
                        else {
                            mCurrCharDelta.x += fontFace->glyph->metrics.horiAdvance * iot->mScaleX + iot->mAdditionalLetterSpace * 64.0;
                        }
//                        mCurrTextHeight = qMax(mCurrTextHeight, static_cast<int>(fontFace->glyph->metrics.vertAdvance * iot->mScaleY));
                    }
                }
            }
        }
        FT_Done_Face(fontFace);
    }

    if (!result)
        mRenderResultInfo = RENDER_ERROR;

    return result;
}

bool FalconRender::textMultPos(ImageObjectTextMultiPos *iotmp)
{
    MemoryFont* memFont = loadFont(iotmp->mFont);

    if (!memFont)
    {
        mRenderResultInfo = RENDER_ERROR;
        return false;
    }

    TextMetrics firsttm, secondtm, numtm;

    if (!getTextMetrics(iotmp->mFirstLine, iotmp->mSizeStr, memFont,
                        iotmp->mScaleXStr, iotmp->mScaleYStr, &firsttm))
        return false;

    if (!getTextMetrics(iotmp->mSecondLine, iotmp->mSizeStr, memFont,
                        iotmp->mScaleXStr, iotmp->mScaleYStr, &secondtm))
        return false;

    int vertOffset;
    int radius;
    int number = iotmp->mNumber;

    ImageObjectText iot;
    iot.mFont = iotmp->mFont;
    iot.mPrintEveryLine = iotmp->mPrintEveryLine;
    iot.mAdditionalLetterSpace = iotmp->mAdditionalLetterSpace;
    iot.mRenderTextWay = iotmp->mRenderTextWay;
    QString num;
    // for picture
    int maxLineWidth = 0;
    int lastLineHeight = 0;
    int firstLineHeight = 0;

    for (int i = 0;i < iotmp->mVertCount;i++)
    {
        for (int j = 0;j < iotmp->mHorCount;j++)
        {
            vertOffset = 0;
            radius = iotmp->mRadius;
            firstLineHeight = 0;

            num = QString("%1%2%3").arg(iotmp->mNumberPrefix).arg(number).arg(iotmp->mNumberPostfix);

            if (!getTextMetrics(num, iotmp->mSizeNum, memFont,
                                iotmp->mScaleXNum, iotmp->mScaleYNum, &numtm))
                return false;

            if (iotmp->mNumberPrefix != "!")
            {
                iot.mScaleX = iotmp->mScaleXNum;
                iot.mScaleY = iotmp->mScaleYNum;
                iot.mSize = iotmp->mSizeNum;
                iot.mText = num;
                iot.mOffsetX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                iot.mOffsetY = iotmp->mOffsetY + i * iotmp->mVertInterval;
                if (radius > 0)
                    iot.mRadius = radius;
                if (!text(&iot, &numtm))
                    return false;

                maxLineWidth = qMax(maxLineWidth, numtm.mWidth);
                lastLineHeight = numtm.mHeight;
                firstLineHeight = numtm.mHeight;
            }

            if (iotmp->mSecondLine.length() > 0)
            {
                if (lastLineHeight > 0) {
                    vertOffset += lastLineHeight / 2 + iotmp->mLinesDistance + secondtm.mHeight / 2;
                    if (radius > 0)
                        radius -= numtm.mHeight + iotmp->mLinesDistance;
                }

                iot.mScaleX = iotmp->mScaleXStr;
                iot.mScaleY = iotmp->mScaleYStr;
                iot.mSize = iotmp->mSizeStr;
                iot.mText = iotmp->mSecondLine;
                iot.mOffsetX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                iot.mOffsetY = iotmp->mOffsetY + i * iotmp->mVertInterval + vertOffset;
                if (radius > 0)
                    iot.mRadius = radius;
                else
                    iot.mRadius = 0;
                if (!text(&iot, &secondtm))
                    return false;

                maxLineWidth = qMax(maxLineWidth, secondtm.mWidth);
                lastLineHeight = secondtm.mHeight;

                if (0 == firstLineHeight)
                    firstLineHeight = secondtm.mHeight;
            }

            if (iotmp->mFirstLine.length() > 0)
            {
                if (lastLineHeight > 0) {
                    vertOffset += lastLineHeight / 2 + iotmp->mLinesDistance + firsttm.mHeight / 2;
                    if (radius > 0)
                        radius -= secondtm.mHeight + iotmp->mLinesDistance;
                }

                iot.mScaleX = iotmp->mScaleXStr;
                iot.mScaleY = iotmp->mScaleYStr;
                iot.mSize = iotmp->mSizeStr;
                iot.mText = iotmp->mFirstLine;
                iot.mOffsetX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                iot.mOffsetY = iotmp->mOffsetY + i * iotmp->mVertInterval + vertOffset;
                if (radius > 0)
                    iot.mRadius = radius;
                else
                    iot.mRadius = 0;
                if (!text(&iot, &firsttm))
                    return false;

                maxLineWidth = qMax(maxLineWidth, firsttm.mWidth);
                lastLineHeight = firsttm.mHeight;
                if (0 == firstLineHeight)
                    firstLineHeight = firsttm.mHeight;
            }

            number += iotmp->mStep;

            // picture
            if (!iotmp->mPicture.mImage.isNull()) {
                switch (iotmp->mPicturePosition) {
                case TOP:
                    iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                    if (lastLineHeight > 0)
                        iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval + vertOffset + lastLineHeight / 2 + iotmp->mPicture.height() / 2 + iotmp->mPictureDrawDistance;
                    else
                        iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval;
                    break;
                case LEFT:
                    if (maxLineWidth > 0)
                        iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval - maxLineWidth / 2 - iotmp->mPicture.width() / 2 - iotmp->mPictureDrawDistance;
                    else
                        iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                    iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval + vertOffset / 2;
                    break;
                case RIGHT:
                    if (maxLineWidth > 0)
                        iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval + maxLineWidth / 2 + iotmp->mPicture.width() / 2 + iotmp->mPictureDrawDistance;
                    else
                        iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                    iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval + vertOffset / 2;
                    break;
                case BOTTOM:
                    iotmp->mPicture.mShiftX = iotmp->mOffsetX + j * iotmp->mHorInterval;
                    if (firstLineHeight > 0)
                        iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval - firstLineHeight / 2 - iotmp->mPicture.height() / 2 - iotmp->mPictureDrawDistance;
                    else
                        iotmp->mPicture.mShiftY = iotmp->mOffsetY + i * iotmp->mVertInterval;
                    break;
                }

                if (!picture(&iotmp->mPicture))
                    return false;
            }

        }
    }
    return true;
}

bool FalconRender::mask(ImageObjectMask *mask)
{
    int left = mask->mOffsetX;
    int bottom = mask->mOffsetY;
    int right = left + mask->mWidth - 1;
    int top = bottom + mask->mHeight - 1;

    if (left >= right || bottom >= top)
        return true;

    if (mask->mDiameter <= mask->mWidth || mask->mDiameter <= mask->mHeight)
    {
        // rendering contour
        if (!moveTo(left, top))
            return false;
        if (!lineTo(right, top))
            return false;
        if (!lineTo(right, bottom))
            return false;
        if (!lineTo(left, bottom))
            return false;
        if (!lineTo(left, top))
            return false;

        if (mask->mHorLinesDistance > 0)
        {
            // rendering horizontal lines
            int vPos = top - mask->mHorLinesDistance;
            int count = 0;
            int from, to;
            while (vPos > bottom && (vPos - bottom) > 0/*mask->mHorLinesDistance*/)
            {
                // interchange moving direction
                from = count % 2 == 0 ? left : right;
                to = count % 2 == 0 ? right : left;
                if (!moveTo(from, vPos))
                    return false;
                if (!lineTo(to, vPos))
                    return false;
                count++;
                vPos -= mask->mHorLinesDistance;
            }
        }

        if (mask->mVertLinesWidth > 0)
        {
            int lleft = static_cast<int>(round(left + mask->mWidth / 2.0 -
                           mask->mVertLinesDistance / 2.0 - mask->mVertLinesWidth / 2.0));
            int lright = static_cast<int>(round(right - mask->mWidth / 2.0 +
                            mask->mVertLinesDistance / 2.0 - mask->mVertLinesWidth / 2.0));

            int hPos = 0;
            int count = 0;
            int from, to;
            while (hPos < mask->mVertLinesWidth)
            {
                from = count % 2 == 0 ? top : bottom;
                to = count % 2 == 0 ? bottom : top;
                if (!moveTo(lleft + hPos, from))
                    return false;
                if (!lineTo(lleft + hPos, to))
                    return false;
                hPos += mask->mVertLinesDrawEveryLine;
                count++;
            }
            // printout last line, so as a result whole line width is filled.
            if (hPos > mask->mVertLinesWidth)
            {
                hPos = mask->mVertLinesWidth - 1;
                from = count % 2 == 0 ? top : bottom;
                to = count % 2 == 0 ? bottom : top;
                if (!moveTo(lleft + hPos, from))
                    return false;
                if (!lineTo(lleft + hPos, to))
                    return false;
            }

            hPos = 0;
            count = 0;
            while (hPos < mask->mVertLinesWidth)
            {
                from = count % 2 == 0 ? top : bottom;
                to = count % 2 == 0 ? bottom : top;
                if (!moveTo(lright + hPos, from))
                    return false;
                if (!lineTo(lright + hPos, to))
                    return false;
                hPos += mask->mVertLinesDrawEveryLine;
                count++;
            }
            // printout last line, so as a result whole line width is filled.
            if (hPos > mask->mVertLinesWidth)
            {
                hPos = mask->mVertLinesWidth - 1;
                from = count % 2 == 0 ? top : bottom;
                to = count % 2 == 0 ? bottom : top;
                if (!moveTo(lright + hPos, from))
                    return false;
                if (!lineTo(lright + hPos, to))
                    return false;
            }
        }
    }
    else
    {
        double radius = mask->mDiameter / 2;
        double y = mask->mHeight / 2;
        double x = mask->mWidth / 2;

        int pointx[8];
        int pointy[8];

        pointx[0] = static_cast<int>(round(-sqrt(radius*radius - y*y)));
        pointy[0] = y;
        pointx[1] = -pointx[0];
        pointy[1] = y;
        pointx[2] = x;
        pointy[2] = static_cast<int>(round(sqrt(radius*radius - x*x)));
        pointx[3] = x;
        pointy[3] = -pointy[2];
        pointx[4] = pointx[1];
        pointy[4] = -y;
        pointx[5] = pointx[0];
        pointy[5] = -y;
        pointx[6] = -x;
        pointy[6] = pointy[3];
        pointx[7] = -x;
        pointy[7] = pointy[2];

        for (int i = 0;i < 8;i++)
        {
            pointx[i] += static_cast<int>(round(x + mask->mOffsetX));
            pointy[i] += static_cast<int>(round(y + mask->mOffsetY));
        }

        int centerx = static_cast<int>(round(mask->mOffsetX + mask->mWidth / 2.0));
        int centery = static_cast<int>(round(mask->mOffsetY + mask->mHeight / 2.0));

        // rendering contour
        if (!moveTo(pointx[0], pointy[0]))
            return false;
        if (!lineTo(pointx[1], pointy[1]))
            return false;
        if (!segmentTo(centerx, centery, pointx[2], pointy[2]))
            return false;
        if (!lineTo(pointx[3], pointy[3]))
            return false;
        if (!segmentTo(centerx, centery, pointx[4], pointy[4]))
            return false;
        if (!lineTo(pointx[5], pointy[5]))
            return false;
        if (!segmentTo(centerx, centery, pointx[6], pointy[6]))
            return false;
        if (!lineTo(pointx[7], pointy[7]))
            return false;
        if (!segmentTo(centerx, centery, pointx[0], pointy[0]))
            return false;

        if (mask->mHorLinesDistance > 0)
        {
            // rendering horizontal lines
            int vPos = top - mask->mHorLinesDistance;
            int count = 0;
            int cleft, cright;
            int from, to;

            while (vPos > bottom && (vPos - bottom) > 0/*mask->mHorLinesDistance*/)
            {
                cleft = static_cast<int>(round(-sqrt(radius*radius -
                       (vPos - bottom - mask->mHeight / 2.0)*(vPos - bottom - mask->mHeight / 2.0))));
                cright = -cleft;

                cleft += mask->mWidth / 2.0 + left;
                cright += mask->mWidth / 2.0 + left;

                if (cleft > left)
                {
                    from = count % 2 == 0 ? cleft : cright;
                    to = count % 2 == 0 ? cright : cleft;
                } else
                {
                    from = count % 2 == 0 ? left : right;
                    to = count % 2 == 0 ? right : left;
                }

                if (!moveTo(from, vPos))
                    return false;
                if (!lineTo(to, vPos))
                    return false;

                count++;
                vPos -= mask->mHorLinesDistance;
            }
        }

        if (mask->mVertLinesWidth > 0)
        {
            int lleft = static_cast<int>(round(left + mask->mWidth / 2.0 -
                           mask->mVertLinesDistance / 2.0 - mask->mVertLinesWidth / 2.0));
            int lright = static_cast<int>(round(right - mask->mWidth / 2.0 +
                            mask->mVertLinesDistance / 2.0 - mask->mVertLinesWidth / 2.0));

            int hPos = 0;
            int count = 0;
            int from, to;
            int ctop, cbottom;

            while (hPos < mask->mVertLinesWidth)
            {
                ctop = static_cast<int>(round(sqrt(radius*radius -
                                              (lleft + hPos - left - mask->mWidth / 2.0)*
                                              (lleft + hPos - left - mask->mWidth / 2.0))));
                cbottom = -ctop;

                ctop += mask->mHeight / 2.0 + bottom;
                cbottom += mask->mHeight / 2.0 + bottom;

                if (ctop > top)
                    ctop = top;
                if (cbottom < bottom)
                    cbottom = bottom;

                from = count % 2 == 0 ? ctop : cbottom;
                to = count % 2 == 0 ? cbottom : ctop;

                if (!moveTo(lleft + hPos, from))
                    return false;
                if (!lineTo(lleft + hPos, to))
                    return false;

                hPos += mask->mVertLinesDrawEveryLine;
                count++;
            }

            // printout last line, so as a result whole line width is filled.
            if (hPos > mask->mVertLinesWidth)
            {
                hPos = mask->mVertLinesWidth - 1;

                ctop = static_cast<int>(round(sqrt(radius*radius -
                                              (lleft + hPos - left - mask->mWidth / 2.0)*
                                              (lleft + hPos - left - mask->mWidth / 2.0))));
                cbottom = -ctop;

                ctop += mask->mHeight / 2.0 + bottom;
                cbottom += mask->mHeight / 2.0 + bottom;

                if (ctop > top)
                    ctop = top;
                if (cbottom < bottom)
                    cbottom = bottom;

                from = count % 2 == 0 ? ctop : cbottom;
                to = count % 2 == 0 ? cbottom : ctop;
                if (!moveTo(lleft + hPos, from))
                    return false;
                if (!lineTo(lleft + hPos, to))
                    return false;
            }

            hPos = 0;
            count = 0;
            while (hPos < mask->mVertLinesWidth)
            {
                int ctop = static_cast<int>(round(sqrt(radius*radius -
                                              (lright + hPos - left - mask->mWidth / 2.0)*
                                              (lright + hPos - left - mask->mWidth / 2.0))));
                double cbottom = -ctop;

                ctop += mask->mHeight / 2.0 + bottom;
                cbottom += mask->mHeight / 2.0 + bottom;

                if (ctop > top)
                    ctop = top;
                if (cbottom < bottom)
                    cbottom = bottom;

                from = count % 2 == 0 ? ctop : cbottom;
                to = count % 2 == 0 ? cbottom : ctop;

                if (!moveTo(lright + hPos, from))
                    return false;
                if (!lineTo(lright + hPos, to))
                    return false;

                hPos += mask->mVertLinesDrawEveryLine;
                count++;
            }
            // printout last line, so as a result whole line width is filled.
            if (hPos > mask->mVertLinesWidth)
            {
                hPos = mask->mVertLinesWidth - 1;

                ctop = static_cast<int>(round(sqrt(radius*radius -
                                              (lright + hPos - left - mask->mWidth / 2.0)*
                                              (lright + hPos - left - mask->mWidth / 2.0))));
                cbottom = -ctop;

                ctop += mask->mHeight / 2.0 + bottom;
                cbottom += mask->mHeight / 2.0 + bottom;

                if (ctop > top)
                    ctop = top;
                if (cbottom < bottom)
                    cbottom = bottom;

                from = count % 2 == 0 ? ctop : cbottom;
                to = count % 2 == 0 ? cbottom : ctop;
                if (!moveTo(lright + hPos, from))
                    return false;
                if (!lineTo(lright + hPos, to))
                    return false;
            }
        }
    }

    return true;
}

bool FalconRender::grid(ImageObjectGrid *grid) {
    int left = grid->mOffsetX;
    int bottom = grid->mOffsetY;
    int right = left + grid->mWidth - 1;
    int top = bottom + grid->mHeight - 1;

    if (left >= right || bottom >= top)
        return true;

    if (grid->mDiameter <= grid->mWidth || grid->mDiameter <= grid->mHeight)
    {
        if (grid->mPrintContour) {
            // rendering contour
            if (!moveTo(left, top))
                return false;
            if (!lineTo(right, top))
                return false;
            if (!lineTo(right, bottom))
                return false;
            if (!lineTo(left, bottom))
                return false;
            if (!lineTo(left, top))
                return false;
        }

        if (grid->mPrintHorLines && grid->mHorLinesDistance > 0)
        {
            // rendering horizontal lines
            int vPos = top - grid->mHorLinesDistance;
            int count = 0;
            int from, to;
            while (vPos > bottom && (vPos - bottom) > 0/*grid->mHorLinesDistance*/)
            {
                // interchange moving direction
                from = count % 2 == 0 ? left : right;
                to = count % 2 == 0 ? right : left;
                if (!moveTo(from, vPos))
                    return false;
                if (!lineTo(to, vPos))
                    return false;
                count++;
                vPos -= grid->mHorLinesDistance;
            }
        }

        if (grid->mPrintVertLines && grid->mVertLinesDistance > 0)
        {
            // rendering vertical lines
            int vPos = left + grid->mVertLinesDistance;
            int count = 0;
            int from, to;
            while (vPos < right && (right - vPos) > 0/*grid->mVertLinesDistance*/)
            {
                // interchange moving direction
                from = count % 2 == 0 ? top : bottom;
                to = count % 2 == 0 ? bottom : top;
                if (!moveTo(vPos, from))
                    return false;
                if (!lineTo(vPos, to))
                    return false;
                count++;
                vPos += grid->mVertLinesDistance;
            }
        }
    }
    else
    {
        double radius = grid->mDiameter / 2;
        double y = grid->mHeight / 2;
        double x = grid->mWidth / 2;

        int pointx[8];
        int pointy[8];

        pointx[0] = static_cast<int>(round(-sqrt(radius*radius - y*y)));
        pointy[0] = y;
        pointx[1] = -pointx[0];
        pointy[1] = y;
        pointx[2] = x;
        pointy[2] = static_cast<int>(round(sqrt(radius*radius - x*x)));
        pointx[3] = x;
        pointy[3] = -pointy[2];
        pointx[4] = pointx[1];
        pointy[4] = -y;
        pointx[5] = pointx[0];
        pointy[5] = -y;
        pointx[6] = -x;
        pointy[6] = pointy[3];
        pointx[7] = -x;
        pointy[7] = pointy[2];

        for (int i = 0;i < 8;i++)
        {
            pointx[i] += static_cast<int>(round(x + grid->mOffsetX));
            pointy[i] += static_cast<int>(round(y + grid->mOffsetY));
        }

        int centerx = static_cast<int>(round(grid->mOffsetX + grid->mWidth / 2.0));
        int centery = static_cast<int>(round(grid->mOffsetY + grid->mHeight / 2.0));

        if (grid->mPrintContour) {
            // rendering contour
            if (!moveTo(pointx[0], pointy[0]))
                return false;
            if (!lineTo(pointx[1], pointy[1]))
                return false;
            if (!segmentTo(centerx, centery, pointx[2], pointy[2]))
                return false;
            if (!lineTo(pointx[3], pointy[3]))
                return false;
            if (!segmentTo(centerx, centery, pointx[4], pointy[4]))
                return false;
            if (!lineTo(pointx[5], pointy[5]))
                return false;
            if (!segmentTo(centerx, centery, pointx[6], pointy[6]))
                return false;
            if (!lineTo(pointx[7], pointy[7]))
                return false;
            if (!segmentTo(centerx, centery, pointx[0], pointy[0]))
                return false;
        }

        if (grid->mPrintHorLines && grid->mHorLinesDistance > 0)
        {
            // rendering horizontal lines
            int vPos = top - grid->mHorLinesDistance;
            int count = 0;
            int cleft, cright;
            int from, to;

            while (vPos > bottom && (vPos - bottom) > 0/*grid->mHorLinesDistance*/)
            {
                cleft = static_cast<int>(round(-sqrt(radius*radius -
                       (vPos - bottom - grid->mHeight / 2.0)*(vPos - bottom - grid->mHeight / 2.0))));
                cright = -cleft;

                cleft += grid->mWidth / 2.0 + left;
                cright += grid->mWidth / 2.0 + left;

                if (cleft > left)
                {
                    from = count % 2 == 0 ? cleft : cright;
                    to = count % 2 == 0 ? cright : cleft;
                } else
                {
                    from = count % 2 == 0 ? left : right;
                    to = count % 2 == 0 ? right : left;
                }

                if (!moveTo(from, vPos))
                    return false;
                if (!lineTo(to, vPos))
                    return false;

                count++;
                vPos -= grid->mHorLinesDistance;
            }
        }

        if (grid->mPrintVertLines && grid->mVertLinesDistance > 0)
        {
            // rendering vertical lines
            int vPos = left + grid->mVertLinesDistance;
            int count = 0;
            int ctop, cbottom;
            int from, to;

            while (vPos < right && (right - vPos) > 0/*grid->mVertLinesDistance*/)
            {
                ctop = static_cast<int>(round(sqrt(radius*radius -
                       (right - vPos - grid->mWidth / 2.0)*(right - vPos - grid->mWidth / 2.0))));
                cbottom = -ctop;

                ctop += grid->mHeight / 2.0 + bottom;
                cbottom += grid->mHeight / 2.0 + bottom;

                if (ctop < top)
                {
                    from = count % 2 == 0 ? ctop : cbottom;
                    to = count % 2 == 0 ? cbottom : ctop;
                } else
                {
                    from = count % 2 == 0 ? top : bottom;
                    to = count % 2 == 0 ? bottom : top;
                }

                if (!moveTo(vPos, from))
                    return false;
                if (!lineTo(vPos, to))
                    return false;

                count++;
                vPos += grid->mHorLinesDistance;
            }
        }
    }

    return true;
}

bool FalconRender::renderImage(ImageData *image, int repeatCount)
{
    int length = image->length();

    for (int repeatCounter = 0; repeatCounter < repeatCount; ++repeatCounter) {
        for (int i = 0;i < length;i++)
        {
            ImageObject* io = image->getObject(i);
            switch(io->mType)
            {
            case TEXT:
                {
                    ImageObjectText *iot = static_cast<ImageObjectText*>(io);
                    if (!text(iot, 0))
                        return false;
                }
                break;
            case TEXT_MULTI_POS:
                {
                    ImageObjectTextMultiPos *iotmp = static_cast<ImageObjectTextMultiPos*>(io);
                    iotmp->mPicture.mCachedImage = QImage();
                    if (!textMultPos(iotmp))
                        return false;
                }
                break;
            case MASK:
                {
                    ImageObjectMask *iom = static_cast<ImageObjectMask*>(io);
                    if (!mask(iom))
                        return false;
                }
                break;
            case PICTURE:
                {
                    ImageObjectPicture *iop = static_cast<ImageObjectPicture*>(io);
                    iop->mCachedImage = QImage();
                    if (!picture(iop))
                        return false;
                }
                break;
            case LINE:
                {
                    ImageObjectLine *iol = static_cast<ImageObjectLine*>(io);

                    if (repeatCounter % 2 == 1 && length == 1) {
                        int fromx = iol->mFromX;
                        int fromy = iol->mFromY;
                        int tox = iol->mToX;
                        int toy = iol->mToY;

                        iol->mFromX = tox;
                        iol->mFromY = toy;
                        iol->mToX = fromx;
                        iol->mToY = fromy;

                        if (!line(iol))
                            return false;

                        iol->mFromX = fromx;
                        iol->mFromY = fromy;
                        iol->mToX = tox;
                        iol->mToY = toy;
                    } else {
                        if (!line(iol))
                            return false;
                    }
                }
                break;
            case GRID:
                {
                    ImageObjectGrid *iog = static_cast<ImageObjectGrid*>(io);

                    grid(iog);
                }
                break;
            }
        }
    }

    return true;
}

RenderResultInfo FalconRender::resultInfo()
{
    return mRenderResultInfo;
}

int move_to( const FT_Vector*  to,
                           void* user )
{
    int toX = to->x / 64.0;
    int toY = to->y / 64.0;

    FalconRender *fr = static_cast<FalconRender*>(user);

    fr->mLastMoveToX = toX;
    fr->mLastMoveToY = toY;

    return !fr->moveTo(toX,toY);
}

int line_to( const FT_Vector*  to,
                            void* user )
{
    int toX = to->x / 64.0;
    int toY = to->y / 64.0;

    FalconRender *fr = static_cast<FalconRender*>(user);

    if (!fr->mIsGlyphClosed)
        if (toX == fr->mLastMoveToX && toY == fr->mLastMoveToY)
            return 0;

    return !fr->lineTo(toX,toY);
}

int conic_to( const FT_Vector*  control,
                             const FT_Vector*  to,
                             void* user )
{
    int controlX = control->x / 64.0;
    int controlY = control->y / 64.0;

    int toX = to->x / 64.0;
    int toY = to->y / 64.0;

    FalconRender *fr = static_cast<FalconRender*>(user);

    if (!fr->mIsGlyphClosed)
        if (toX == fr->mLastMoveToX && toY == fr->mLastMoveToY)
            return 0;

    return !fr->conicTo(controlX,controlY,toX,toY);
}

int cubic_to( const FT_Vector*  control1,
                             const FT_Vector*  control2,
                             const FT_Vector*  to,
                             void* user )
{
    int control1X = control1->x / 64.0;
    int control1Y = control1->y / 64.0;
    int control2X = control2->x / 64.0;
    int control2Y = control2->y / 64.0;

    int toX = to->x / 64.0;
    int toY = to->y / 64.0;

    FalconRender *fr = static_cast<FalconRender*>(user);

    if (!fr->mIsGlyphClosed)
        if (toX == fr->mLastMoveToX && toY == fr->mLastMoveToY)
            return 0;

    return !fr->cubicTo(control1X,control1Y,control2X,control2Y,toX,toY);
}

void raster(const int y, const int count,
                    const FT_Span * const spans, void * const user)
{
    FalconRender* falconRender = static_cast<FalconRender*>(user);
    falconRender->onRaster(y, count, spans);
}
