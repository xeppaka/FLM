#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QString>
#include <QList>
#include <QImage>

/***************************************/
/* Default values for major variables  */
/*                                     */
#define FLM_FONT "flm_font"
#define INTERNAL_FONT ":/lama/fonts/arial.ttf"

enum ImageObjectType
{
    OBJECT_UNDEFINED = 0,
    LINE,
    TEXT,
    TEXT_MULTI_POS,
    MASK,
    PICTURE,
    GRID
};

enum RenderTextWay
{
    RENDER_OUTLINE = 0,
    RENDER_INLINE,
    RENDER_BOTH
};

enum ImageActiveSize {
    SIZE_SCALED = 0,
    SIZE_FIXED
};

enum ImageDrawPosition {
    TOP = 0,
    RIGHT,
    BOTTOM,
    LEFT
};

struct ImageObject
{
public:
    ImageObjectType mType;
};

struct ImageObjectLine : public ImageObject
{
public:
    ImageObjectLine(){mType = LINE;}
    int mFromX,mFromY;
    int mToX,mToY;
};

struct ImageObjectPicture : public ImageObject
{
public:
    ImageObjectPicture()
    {
        mType = PICTURE;
        mActiveSize = SIZE_SCALED;
        mScaleX = mScaleY = 1.0f;
        mWidth = mHeight = 0;
        mShiftX = mShiftY = 0;
        mDrawEveryLine = 30;
    }

    QImage mImage;
    QImage mCachedImage;
    float mScaleX, mScaleY;
    int mWidth, mHeight;
    int mShiftX, mShiftY;
    int mDrawEveryLine;
    ImageActiveSize mActiveSize;

    int height();
    int width();
};

struct ImageObjectText : public ImageObject
{
public:
    ImageObjectText()
    {
        mType = TEXT;
        mText = "";
        mFont = INTERNAL_FONT;
        mOffsetX = mOffsetY = 0;
        mSize = 0;
        mRenderTextWay = RENDER_INLINE;
        mPrintEveryLine = 30;
        mRadius = 0;
        mScaleX = mScaleY = 0;
        mAdditionalLetterSpace = 0;
    }
    QString mText;
    QString mFont;
    int mOffsetX, mOffsetY;
    float mSize;
    RenderTextWay mRenderTextWay;
    int mPrintEveryLine;
    int mRadius;
    int mAdditionalLetterSpace;
    float mScaleX, mScaleY;
};

struct ImageObjectTextMultiPos : public ImageObject
{
public:
    ImageObjectTextMultiPos()
    {
        mType = TEXT_MULTI_POS;
        mFirstLine = "";
        mSecondLine = "";
        mNumberPrefix = "";
        mNumberPostfix = "";
        mNumber = 0;
        mStep = 0;
        mVertCount = mHorCount = 0;
        mVertInterval = mHorInterval = 0;
        mFont = INTERNAL_FONT;
        mOffsetX = mOffsetY = 0;
        mSizeStr = mSizeNum = 0;
        mScaleXStr = mScaleYStr = mScaleXNum = mScaleYNum = 0;
        mRenderTextWay = RENDER_INLINE;
        mPrintEveryLine = 30;
        mLinesDistance = 1000;
        mRadius = 0;
        mAdditionalLetterSpace = 0;
    }
    QString mFirstLine, mSecondLine;
    QString mNumberPrefix, mNumberPostfix;
    int mNumber;
    int mStep;
    int mVertCount,mHorCount;
    int mVertInterval,mHorInterval;
    QString mFont;
    int mOffsetX,mOffsetY;
    float mSizeStr, mSizeNum;
    float mScaleXStr, mScaleYStr;
    float mScaleXNum, mScaleYNum;
    RenderTextWay mRenderTextWay;
    int mPrintEveryLine;
    int mLinesDistance;
    int mRadius;
    int mAdditionalLetterSpace;
    // picture
    int mPictureDrawDistance;
    ImageDrawPosition mPicturePosition;
    ImageObjectPicture mPicture;
};

struct ImageObjectMask : public ImageObject
{
public:
    ImageObjectMask()
    {
        mType = MASK;
        mOffsetX = mOffsetY = 0;
        mWidth = mHeight = 1;
        mDiameter = 0;
        mHorLinesDistance = mVertLinesDistance = mVertLinesWidth = mVertLinesDrawEveryLine = 0;
    }
    int mOffsetX, mOffsetY;
    int mWidth, mHeight;
    int mDiameter;
    int mHorLinesDistance;
    int mVertLinesDistance, mVertLinesWidth;
    int mVertLinesDrawEveryLine;
};

struct ImageObjectGrid : public ImageObject
{
public:
    ImageObjectGrid()
    {
        mType = GRID;
        mOffsetX = mOffsetY = 0;
        mWidth = mHeight = 1;
        mDiameter = 0;
        mHorLinesDistance = mVertLinesDistance = 0;
        mPrintHorLines = mPrintVertLines = mPrintContour = true;
    }

    int mOffsetX, mOffsetY;
    int mWidth, mHeight;
    int mDiameter;
    int mHorLinesDistance;
    int mVertLinesDistance;
    bool mPrintContour;
    bool mPrintHorLines;
    bool mPrintVertLines;
};

class ImageData
{
public:
    ImageData();
    ~ImageData();

    void addObject(ImageObject* object);
    ImageObject* getObject(int index);
    void removeObject(int index);
    void free();
    int length();

private:
    QList<ImageObject*> mData;
};

#endif // IMAGEDATA_H
