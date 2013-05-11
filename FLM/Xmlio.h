#ifndef XMLIO_H
#define XMLIO_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlSimpleReader>
#include "falconrender.h"

#define CURR_VER "1.0.0"

enum Element
{
    ELEMENT_UNDEFINED = 0,
    TEXT_VAL,
    FONT_VAL,
    OFFSET_X,
    OFFSET_Y,
    SIZE_ELEM,
    RENDER_WAY,
    PRINT_EVERY_LINE,
    FIRST_LINE,
    SECOND_LINE,
    START_NUM,
    STEP,
    SIZE_STR,
    SIZE_NUM,
    HOR_COUNT,
    VERT_COUNT,
    HOR_INTERVAL,
    VERT_INTERVAL,
    LINES_DIST,
    SCALE_X_NUM,
    SCALE_Y_NUM,
    SCALE_X_STR,
    SCALE_Y_STR,
    WIDTH,
    HEIGHT,
    DIAMETER,
    HOR_LINES_DIST,
    VERT_LINES_DIST,
    VERT_LINES_WIDTH,
    VERT_LINES_PRINT_EVERY_LINE,
    NUMBER_PREFIX,
    NUMBER_POSTFIX,
    ADDITIONAL_LETTER_SPACE,
    RADIUS,
    PICTURE_DRAW_DISTANCE,
    PICTURE_POSITION,
    PICTURE_ACTIVE_SIZE,
    PICTURE_DRAW_EVERY_LINE,
    PICTURE_HEIGHT,
    PICTURE_WIDTH,
    PICTURE_SCALEX,
    PICTURE_SCALEY,
    PICTURE_SHIFTX,
    PICTURE_SHIFTY,
    PICTURE_DATA,
    FROM_X,
    FROM_Y,
    TO_X,
    TO_Y,
    PRINT_HOR_LINES,
    PRINT_VERT_LINES,
    PRINT_CONTOUR
};

class Xmlio : public QXmlDefaultHandler
{
public:
    Xmlio();

private:
    virtual bool startDocument();
    virtual bool endDocument ();
    virtual bool startElement ( const QString &namespaceURI, const QString &localName,
                                const QString &qName, const QXmlAttributes &atts );
    virtual bool endElement ( const QString &namespaceURI, const QString &localName,
                              const QString &qName );
    virtual bool characters ( const QString & ch );

    ImageData* mImageData;
    ImageObject* mio;
    ImageObjectType mCurrObjectType;
    Element mCurrElement;

public:
    bool load(QString& fileName, ImageData* idata);
    bool save(QString& fileName, ImageData* idata);
};

#endif // XMLIO_H
