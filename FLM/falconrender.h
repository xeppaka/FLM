#ifndef FALCONRENDER_H
#define FALCONRENDER_H

#include <cmath>
#include "outputdevice.h"
#include "imagedata.h"
#include "flmsettings.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QSet>
#include <QMatrix>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H

typedef QVector<QPoint> pointsvector;
typedef QMap<int, pointsvector*> spansmap;

const double Pi = 3.1415926535897932384626433832795f;

struct MemoryFont
{
    FT_Byte* mData;
    int mSize;
    bool mFLMFont;
};

struct TextMetrics
{
    int mWidth;
    int mHeight;
    int mSpaceBefore;
    int mSpaceAfter;
};

enum ValidatePointIsOutResult
{
    POINT_OUT = 0,
    POINT_IN,
    POINT_IN_CONNECT
};

enum RenderResultInfo
{
    RENDER_OK = 0,
    RENDER_OK_OUT_FIELD,
    RENDER_ERROR
};

class FalconRender : public QObject
{
    Q_OBJECT

public:
    FalconRender();
    ~FalconRender();

private:
    OutputDevice *mOutDevice;
    FLMSettings *mFLMSettings;
    RenderConfig *mRenderConfig;


    // Laser state
    bool mLaserOn;
    int mCurPosX, mCurPosY;
    int mLastMoveToX,mLastMoveToY;
    bool mIsGlyphClosed;
    bool mPrevPointIsOut;
    int mLastInPointX, mLastInPointY;
//    int mCurrTextHeight;
    bool mFastDraw;
    int mPrintEveryLine;
    FT_Vector mCurrCharDelta;
    RenderResultInfo mRenderResultInfo;

    // raster spans
    spansmap *mSpans;
    int mLastY;

    // Glyphs which shouldn't be closed. Only for our font
    QSet<wchar_t> mClosedGlyphs;

    QMap<QString, MemoryFont*> mFonts;
    QMatrix* mCorrMatrix;

    // freetype library variables
    FT_Library mFTLibrary;
    FT_Outline_Funcs mFTOutlineCallbacks;
    
//private:
public:
    // approximate length of second order beizer arc
    double getConicLength(int fromX,
                         int fromY,
                         int controlX,
                         int controlY,
                         int toX,
                         int toY);

    // approximate length of third order beizer arc
    double getCubicLength(int fromX,
                        int fromY,
                        int control1X,
                        int control1Y,
                        int control2X,
                        int control2Y,
                        int toX,
                        int toY);

    // coordinates range validation
    ValidatePointIsOutResult validatePointIsOut(int x, int y);
    void onRaster(const int y, const int count, const FT_Span * const spans);
    void allocateSpans();
    bool rasterSpans();
    void releaseSpans();
    MemoryFont* loadFont(QString& fontPath);
    bool getTextMetrics(QString& text, int size, MemoryFont* font, float scaleX, float scaleY,
                        TextMetrics* metrics);
    void correctPoint(int* x,int *y);

public:
    bool begin(OutputDevice* outDevice, FLMSettings* settings, QMatrix* corrMatrix = 0,
               bool fastDraw = false);
    bool end();

    // Simple figures
    bool moveTo(int x,
                int y);

    bool lineTo(int x,
                int y);

    // Beizer arcs
    bool conicTo(int controlX,
                 int controlY,
                 int toX,
                 int toY);

    bool cubicTo(int control1X,
                 int control1Y,
                 int control2X,
                 int control2Y,
                 int toX,
                 int toY);
    bool segmentTo(int centerX, int centerY, int toX, int toY);

    bool text(ImageObjectText *iot, TextMetrics *tm);
    bool textMultPos(ImageObjectTextMultiPos *iotmp);
    bool mask(ImageObjectMask *mask);
    bool picture(ImageObjectPicture *picture);
    bool line(ImageObjectLine *line);
    bool grid(ImageObjectGrid *grid);

    bool renderImage(ImageData* image, int repeatCount);

    RenderResultInfo resultInfo();

    friend int move_to( const FT_Vector*  to,
                        void* user );
    friend int line_to( const FT_Vector*  to,
                        void* user );
    friend int conic_to( const FT_Vector*  control,
                         const FT_Vector*  to,
                         void* user );
    friend int cubic_to( const FT_Vector*  control1,
                         const FT_Vector*  control2,
                         const FT_Vector*  to,
                         void* user );
    friend void raster(const int y, const int count,
                  const FT_Span * const spans, void * const user);
};

#endif // FALCONRENDER_H
