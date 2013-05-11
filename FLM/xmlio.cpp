#include "Xmlio.h"
#include <QBuffer>

Xmlio::Xmlio()
{
    mCurrObjectType = OBJECT_UNDEFINED;
}

bool Xmlio::save(QString& fileName, ImageData* idata)
{
    if (fileName.length() <= 0 || idata == 0)
        return false;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument(CURR_VER);
    xmlWriter.writeStartElement("", QObject::tr("ImageData"));

    for (int i = 0;i < idata->length();i++)
    {
        ImageObject* io = idata->getObject(i);
        ImageObjectText* iot;
        ImageObjectTextMultiPos* iotmp;
        ImageObjectMask* iom;
        ImageObjectLine* iol;
        ImageObjectGrid* iog;

        switch (io->mType)
        {
        case TEXT:
            iot = static_cast<ImageObjectText*>(io);
            xmlWriter.writeStartElement("", QObject::tr("ImageObject"));
            xmlWriter.writeAttribute("", QObject::tr("type"), QObject::tr("TEXT"));
            xmlWriter.writeStartElement("", QObject::tr("text_val"));
            xmlWriter.writeCharacters(iot->mText);
            xmlWriter.writeEndElement();
            if (iot->mFont.compare(INTERNAL_FONT,Qt::CaseSensitive) != 0)
            {
                xmlWriter.writeStartElement("", QObject::tr("font_val"));
                xmlWriter.writeCharacters(iot->mFont);
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeStartElement("", QObject::tr("offset_x"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mOffsetX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("offset_y"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mOffsetY));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("size"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mSize));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("additional_letter_space"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mAdditionalLetterSpace));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("radius"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mRadius));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("render_way"));
            xmlWriter.writeCharacters(QString("%1").arg(iot->mRenderTextWay));
            xmlWriter.writeEndElement();

            if (iot->mRenderTextWay == RENDER_INLINE || iot->mRenderTextWay == RENDER_BOTH)
            {
                xmlWriter.writeStartElement("", QObject::tr("print_every_line"));
                xmlWriter.writeCharacters(QString("%1").arg(iot->mPrintEveryLine));
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeEndElement();
            break;
        case TEXT_MULTI_POS:
            iotmp = static_cast<ImageObjectTextMultiPos*>(io);
            xmlWriter.writeStartElement("",QObject::tr("ImageObject"));
            xmlWriter.writeAttribute("", QObject::tr("type"), QObject::tr("TEXT_MULT"));
            xmlWriter.writeStartElement("", QObject::tr("first_line"));
            xmlWriter.writeCharacters(iotmp->mFirstLine);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("second_line"));
            xmlWriter.writeCharacters(iotmp->mSecondLine);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("number_prefix"));
            xmlWriter.writeCharacters(iotmp->mNumberPrefix);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("start_num"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mNumber));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("number_postfix"));
            xmlWriter.writeCharacters(iotmp->mNumberPostfix);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("step"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mStep));
            xmlWriter.writeEndElement();
            if (iotmp->mFont.compare(INTERNAL_FONT,Qt::CaseSensitive) != 0)
            {
                xmlWriter.writeStartElement("", QObject::tr("font_val"));
                xmlWriter.writeCharacters(iotmp->mFont);
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeStartElement("", QObject::tr("size_str"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mSizeStr));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("size_num"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mSizeNum));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("offset_x"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mOffsetX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("offset_y"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mOffsetY));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("hor_count"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mHorCount));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vert_count"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mVertCount));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("hor_interval"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mHorInterval));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vert_interval"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mVertInterval));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("lines_dist"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mLinesDistance));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("scale_x_num"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mScaleXNum));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("scale_y_num"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mScaleYNum));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("scale_x_str"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mScaleXStr));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("scale_y_str"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mScaleYStr));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("additional_letter_space"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mAdditionalLetterSpace));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("radius"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mRadius));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("render_way"));
            xmlWriter.writeCharacters(QString("%1").arg(iotmp->mRenderTextWay));
            xmlWriter.writeEndElement();
            if (iotmp->mRenderTextWay == RENDER_INLINE || iotmp->mRenderTextWay == RENDER_BOTH)
            {
                xmlWriter.writeStartElement("", QObject::tr("print_every_line"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPrintEveryLine));
                xmlWriter.writeEndElement();
            }
            // picture
            if (!iotmp->mPicture.mImage.isNull()) {
                xmlWriter.writeStartElement("", QObject::tr("picture_draw_distance"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPictureDrawDistance));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_position"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicturePosition));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_active_size"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mActiveSize));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_draw_every_line"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mDrawEveryLine));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_height"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mHeight));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_width"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mWidth));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_scaleX"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mScaleX));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_scaleY"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mScaleY));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture_scaleY"));
                xmlWriter.writeCharacters(QString("%1").arg(iotmp->mPicture.mScaleY));
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("", QObject::tr("picture"));
                {
                    QByteArray ba;
                    QBuffer buf(&ba);
                    buf.open(QBuffer::ReadWrite);
                    iotmp->mPicture.mImage.save(&buf, "PNG");
                    buf.close();
                    QString string(ba.toBase64());
                    xmlWriter.writeCharacters(string);
                }
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeEndElement();
            break;
        case MASK:
            iom = static_cast<ImageObjectMask*>(io);
            xmlWriter.writeStartElement("",QObject::tr("ImageObject"));
            xmlWriter.writeAttribute("", QObject::tr("type"), QObject::tr("MASK"));
            xmlWriter.writeStartElement("", QObject::tr("width"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mWidth));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("height"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mHeight));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("diameter"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mDiameter));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("offset_x"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mOffsetX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("offset_y"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mOffsetY));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("hor_lines_distance"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mHorLinesDistance));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vert_lines_distance"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mVertLinesDistance));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vert_lines_width"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mVertLinesWidth));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vert_lines_print_every_line"));
            xmlWriter.writeCharacters(QString("%1").arg(iom->mVertLinesDrawEveryLine));
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            break;
        case LINE:
            iol = static_cast<ImageObjectLine*>(io);
            xmlWriter.writeStartElement("",QObject::tr("ImageObject"));
            xmlWriter.writeAttribute("", QObject::tr("type"), QObject::tr("LINE"));
            xmlWriter.writeStartElement("", QObject::tr("fromx"));
            xmlWriter.writeCharacters(QString("%1").arg(iol->mFromX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("fromy"));
            xmlWriter.writeCharacters(QString("%1").arg(iol->mFromY));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("tox"));
            xmlWriter.writeCharacters(QString("%1").arg(iol->mToX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("toy"));
            xmlWriter.writeCharacters(QString("%1").arg(iol->mToY));
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            break;
        case GRID:
            iog = static_cast<ImageObjectGrid*>(io);
            xmlWriter.writeStartElement("",QObject::tr("ImageObject"));
            xmlWriter.writeAttribute("", QObject::tr("type"), QObject::tr("GRID"));
            xmlWriter.writeStartElement("", QObject::tr("fromx"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mOffsetX));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("fromy"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mOffsetY));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("width"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mWidth));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("height"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mHeight));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("diameter"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mDiameter));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("horizontal_lines_distance"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mHorLinesDistance));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("vertical_lines_distance"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mVertLinesDistance));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("print_contour"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mPrintContour));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("print_horizontal_lines"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mPrintHorLines));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("", QObject::tr("print_vertical_lines"));
            xmlWriter.writeCharacters(QString("%1").arg(iog->mPrintVertLines));
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            break;
        }
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();
    return true;
}

bool Xmlio::load(QString& fileName, ImageData* idata)
{
    if (fileName.length() <= 0 || idata == 0)
        return false;

    bool result = false;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        mImageData = idata;
        QXmlInputSource xmlInput(&file);
        QXmlSimpleReader xmlReader;
        xmlReader.setContentHandler(this);
        xmlReader.setErrorHandler(this);
        result = xmlReader.parse(xmlInput);
        file.close();
        mImageData = 0;
    }
    return result;
}

bool Xmlio::startDocument()
{
    return true;
}

bool Xmlio::endDocument()
{
    return true;
}

bool Xmlio::startElement(const QString &namespaceURI, const QString &localName,
                         const QString &qName, const QXmlAttributes &atts)
{
    if (localName.compare("ImageData") == 0)
        return true;

    if (localName.compare("ImageObject") == 0 && mCurrObjectType == OBJECT_UNDEFINED)
    {
        mCurrElement = ELEMENT_UNDEFINED;
        QString objType = atts.value("", QObject::tr("type"));
        if (objType.compare(QObject::tr("TEXT")) == 0)
        {
            mCurrObjectType = TEXT;
            mio = new ImageObjectText();
        }
        else if (objType.compare(QObject::tr("TEXT_MULT")) == 0)
        {
            mCurrObjectType = TEXT_MULTI_POS;
            mio = new ImageObjectTextMultiPos();
        }
        else if (objType.compare(QObject::tr("MASK")) == 0)
        {
            mCurrObjectType = MASK;
            mio = new ImageObjectMask();
        }
        else if (objType.compare(QObject::tr("LINE")) == 0)
        {
            mCurrObjectType = LINE;
            mio = new ImageObjectLine();
        }
        else if (objType.compare(QObject::tr("GRID")) == 0)
        {
            mCurrObjectType = GRID;
            mio = new ImageObjectGrid();
        }

        return true;
    }

    switch (mCurrObjectType)
    {
    case TEXT:
        if (localName.compare(QObject::tr("text_val")) == 0)
            mCurrElement = TEXT_VAL;
        else if (localName.compare(QObject::tr("font_val")) == 0)
            mCurrElement = FONT_VAL;
        else if (localName.compare(QObject::tr("offset_x")) == 0)
            mCurrElement = OFFSET_X;
        else if (localName.compare(QObject::tr("offset_y")) == 0)
            mCurrElement = OFFSET_Y;
        else if (localName.compare(QObject::tr("size")) == 0)
            mCurrElement = SIZE_ELEM;
        else if (localName.compare(QObject::tr("render_way")) == 0)
            mCurrElement = RENDER_WAY;
        else if (localName.compare(QObject::tr("print_every_line")) == 0)
            mCurrElement = PRINT_EVERY_LINE;
        else if (localName.compare(QObject::tr("additional_letter_space")) == 0)
            mCurrElement = ADDITIONAL_LETTER_SPACE;
        else if (localName.compare(QObject::tr("radius")) == 0)
            mCurrElement = RADIUS;
        break;
    case TEXT_MULTI_POS:
        if (localName.compare(QObject::tr("first_line")) == 0)
            mCurrElement = FIRST_LINE;
        else if (localName.compare(QObject::tr("second_line")) == 0)
            mCurrElement = SECOND_LINE;
        else if (localName.compare(QObject::tr("start_num")) == 0)
            mCurrElement = START_NUM;
        else if (localName.compare(QObject::tr("step")) == 0)
            mCurrElement = STEP;
        else if (localName.compare(QObject::tr("font_val")) == 0)
            mCurrElement = FONT_VAL;
        else if (localName.compare(QObject::tr("offset_x")) == 0)
            mCurrElement = OFFSET_X;
        else if (localName.compare(QObject::tr("offset_y")) == 0)
            mCurrElement = OFFSET_Y;
        else if (localName.compare(QObject::tr("hor_count")) == 0)
            mCurrElement = HOR_COUNT;
        else if (localName.compare(QObject::tr("vert_count")) == 0)
            mCurrElement = VERT_COUNT;
        else if (localName.compare(QObject::tr("hor_interval")) == 0)
            mCurrElement = HOR_INTERVAL;
        else if (localName.compare(QObject::tr("vert_interval")) == 0)
            mCurrElement = VERT_INTERVAL;
        else if (localName.compare(QObject::tr("lines_dist")) == 0)
            mCurrElement = LINES_DIST;
        else if (localName.compare(QObject::tr("scale_x_num")) == 0)
            mCurrElement = SCALE_X_NUM;
        else if (localName.compare(QObject::tr("scale_y_num")) == 0)
            mCurrElement = SCALE_Y_NUM;
        else if (localName.compare(QObject::tr("scale_x_str")) == 0)
            mCurrElement = SCALE_X_STR;
        else if (localName.compare(QObject::tr("scale_y_str")) == 0)
            mCurrElement = SCALE_Y_STR;
        else if (localName.compare(QObject::tr("render_way")) == 0)
            mCurrElement = RENDER_WAY;
        else if (localName.compare(QObject::tr("print_every_line")) == 0)
            mCurrElement = PRINT_EVERY_LINE;
        else if (localName.compare(QObject::tr("size_str")) == 0)
            mCurrElement = SIZE_STR;
        else if (localName.compare(QObject::tr("size_num")) == 0)
            mCurrElement = SIZE_NUM;
        else if (localName.compare(QObject::tr("number_prefix")) == 0)
            mCurrElement = NUMBER_PREFIX;
        else if (localName.compare(QObject::tr("number_postfix")) == 0)
            mCurrElement = NUMBER_POSTFIX;
        else if (localName.compare(QObject::tr("additional_letter_space")) == 0)
            mCurrElement = ADDITIONAL_LETTER_SPACE;
        else if (localName.compare(QObject::tr("radius")) == 0)
            mCurrElement = RADIUS;
        else if (localName.compare(QObject::tr("picture_draw_distance")) == 0)
            mCurrElement = PICTURE_DRAW_DISTANCE;
        else if (localName.compare(QObject::tr("picture_position")) == 0)
            mCurrElement = PICTURE_POSITION;
        else if (localName.compare(QObject::tr("picture_active_size")) == 0)
            mCurrElement = PICTURE_ACTIVE_SIZE;
        else if (localName.compare(QObject::tr("picture_draw_every_line")) == 0)
            mCurrElement = PICTURE_DRAW_EVERY_LINE;
        else if (localName.compare(QObject::tr("picture_height")) == 0)
            mCurrElement = PICTURE_HEIGHT;
        else if (localName.compare(QObject::tr("picture_width")) == 0)
            mCurrElement = PICTURE_WIDTH;
        else if (localName.compare(QObject::tr("picture_scaleX")) == 0)
            mCurrElement = PICTURE_SCALEX;
        else if (localName.compare(QObject::tr("picture_scaleY")) == 0)
            mCurrElement = PICTURE_SCALEY;
        else if (localName.compare(QObject::tr("picture")) == 0)
            mCurrElement = PICTURE_DATA;
        break;
    case MASK:
        if (localName.compare(QObject::tr("width")) == 0)
            mCurrElement = WIDTH;
        else if (localName.compare(QObject::tr("height")) == 0)
            mCurrElement = HEIGHT;
        else if (localName.compare(QObject::tr("diameter")) == 0)
            mCurrElement = DIAMETER;
        else if (localName.compare(QObject::tr("offset_x")) == 0)
            mCurrElement = OFFSET_X;
        else if (localName.compare(QObject::tr("offset_y")) == 0)
            mCurrElement = OFFSET_Y;
        else if (localName.compare(QObject::tr("hor_lines_distance")) == 0)
            mCurrElement = HOR_LINES_DIST;
        else if (localName.compare(QObject::tr("vert_lines_distance")) == 0)
            mCurrElement = VERT_LINES_DIST;
        else if (localName.compare(QObject::tr("vert_lines_width")) == 0)
            mCurrElement = VERT_LINES_WIDTH;
        else if (localName.compare(QObject::tr("vert_lines_print_every_line")) == 0)
            mCurrElement = VERT_LINES_PRINT_EVERY_LINE;
        break;
    case LINE:
        if (localName.compare(QObject::tr("fromx")) == 0)
            mCurrElement = FROM_X;
        else if (localName.compare(QObject::tr("fromy")) == 0)
            mCurrElement = FROM_Y;
        else if (localName.compare(QObject::tr("tox")) == 0)
            mCurrElement = TO_X;
        else if (localName.compare(QObject::tr("toy")) == 0)
            mCurrElement = TO_Y;
        break;
    case GRID:
        if (localName.compare(QObject::tr("fromx")) == 0)
            mCurrElement = FROM_X;
        else if (localName.compare(QObject::tr("fromy")) == 0)
            mCurrElement = FROM_Y;
        else if (localName.compare(QObject::tr("width")) == 0)
            mCurrElement = WIDTH;
        else if (localName.compare(QObject::tr("height")) == 0)
            mCurrElement = HEIGHT;
        else if (localName.compare(QObject::tr("diameter")) == 0)
            mCurrElement = DIAMETER;
        else if (localName.compare(QObject::tr("horizontal_lines_distance")) == 0)
            mCurrElement = HOR_LINES_DIST;
        else if (localName.compare(QObject::tr("vertical_lines_distance")) == 0)
            mCurrElement = VERT_LINES_DIST;
        else if (localName.compare(QObject::tr("print_contour")) == 0)
            mCurrElement = PRINT_CONTOUR;
        else if (localName.compare(QObject::tr("print_horizontal_lines")) == 0)
            mCurrElement = PRINT_HOR_LINES;
        else if (localName.compare(QObject::tr("print_vertical_lines")) == 0)
            mCurrElement = PRINT_VERT_LINES;
        break;
    }

    return true;
}

bool Xmlio::endElement(const QString &namespaceURI, const QString &localName,
                       const QString &qName)
{
    if (localName.compare(QObject::tr("ImageObject")) == 0)
    {
        if (mio)
            mImageData->addObject(mio);
        mCurrObjectType = OBJECT_UNDEFINED;
    } else
    {
        mCurrElement = ELEMENT_UNDEFINED;
    }

    return true;
}

bool Xmlio::characters(const QString &ch)
{
    if (mCurrElement == ELEMENT_UNDEFINED)
        return true;

    ImageObjectText* iot;
    ImageObjectTextMultiPos* iotmp;
    ImageObjectMask* iom;
    ImageObjectLine* iol;
    ImageObjectGrid* iog;

    switch (mCurrObjectType)
    {
    case TEXT:
        iot = static_cast<ImageObjectText*>(mio);
        switch (mCurrElement)
        {
        case TEXT_VAL:
            iot->mText = ch;
            break;
        case FONT_VAL:
            iot->mFont = ch;
            break;
        case OFFSET_X:
            iot->mOffsetX = ch.toInt();
            break;
        case OFFSET_Y:
            iot->mOffsetY = ch.toInt();
            break;
        case SIZE_ELEM:
            iot->mSize = ch.toFloat();
            break;
        case RENDER_WAY:
            iot->mRenderTextWay = static_cast<RenderTextWay>(ch.toInt());
            break;
        case PRINT_EVERY_LINE:
            iot->mPrintEveryLine = ch.toInt();
            break;
        case ADDITIONAL_LETTER_SPACE:
            iot->mAdditionalLetterSpace = ch.toInt();
            break;
        case RADIUS:
            iot->mRadius = ch.toInt();
            break;
        }

        mCurrElement = ELEMENT_UNDEFINED;
        break;
    case TEXT_MULTI_POS:
        iotmp = static_cast<ImageObjectTextMultiPos*>(mio);
        switch (mCurrElement)
        {
        case FIRST_LINE:
            iotmp->mFirstLine = ch;
            break;
        case SECOND_LINE:
            iotmp->mSecondLine = ch;
            break;
        case START_NUM:
            iotmp->mNumber = ch.toInt();
            break;
        case STEP:
            iotmp->mStep = ch.toInt();
            break;
        case FONT_VAL:
            iotmp->mFont = ch;
            break;
        case OFFSET_X:
            iotmp->mOffsetX = ch.toInt();
            break;
        case OFFSET_Y:
            iotmp->mOffsetY = ch.toInt();
            break;
        case HOR_COUNT:
            iotmp->mHorCount = ch.toInt();
            break;
        case VERT_COUNT:
            iotmp->mVertCount = ch.toInt();
            break;
        case HOR_INTERVAL:
            iotmp->mHorInterval = ch.toInt();
            break;
        case VERT_INTERVAL:
            iotmp->mVertInterval = ch.toInt();
            break;
        case LINES_DIST:
            iotmp->mLinesDistance = ch.toInt();
            break;
        case SCALE_X_NUM:
            iotmp->mScaleXNum = ch.toFloat();
            break;
        case SCALE_Y_NUM:
            iotmp->mScaleYNum = ch.toFloat();
            break;
        case SCALE_X_STR:
            iotmp->mScaleXStr = ch.toFloat();
            break;
        case SCALE_Y_STR:
            iotmp->mScaleYStr = ch.toFloat();
            break;
        case RENDER_WAY:
            iotmp->mRenderTextWay = static_cast<RenderTextWay>(ch.toInt());
            break;
        case PRINT_EVERY_LINE:
            iotmp->mPrintEveryLine = ch.toInt();
            break;
        case SIZE_STR:
            iotmp->mSizeStr = ch.toFloat();
            break;
        case SIZE_NUM:
            iotmp->mSizeNum = ch.toFloat();
            break;
        case NUMBER_PREFIX:
            iotmp->mNumberPrefix = ch;
            break;
        case NUMBER_POSTFIX:
            iotmp->mNumberPostfix = ch;
            break;
        case ADDITIONAL_LETTER_SPACE:
            iotmp->mAdditionalLetterSpace = ch.toInt();
            break;
        case RADIUS:
            iotmp->mRadius = ch.toInt();
            break;
        case PICTURE_DRAW_DISTANCE:
            iotmp->mPictureDrawDistance = ch.toInt();
            break;
        case PICTURE_POSITION:
            iotmp->mPicturePosition = (ImageDrawPosition)ch.toInt();
            break;
        case PICTURE_ACTIVE_SIZE:
            iotmp->mPicture.mActiveSize = (ImageActiveSize)ch.toInt();
            break;
        case PICTURE_DRAW_EVERY_LINE:
            iotmp->mPicture.mDrawEveryLine = ch.toInt();
            break;
        case PICTURE_HEIGHT:
            iotmp->mPicture.mHeight = ch.toInt();
            break;
        case PICTURE_WIDTH:
            iotmp->mPicture.mWidth = ch.toInt();
            break;
        case PICTURE_SCALEX:
            iotmp->mPicture.mScaleX = ch.toFloat();
            break;
        case PICTURE_SCALEY:
            iotmp->mPicture.mScaleY = ch.toFloat();
            break;
        case PICTURE_DATA:
            {
                QByteArray ba = QByteArray::fromBase64(ch.toAscii());
                iotmp->mPicture.mImage.loadFromData(ba, "PNG");
                break;
            }
        }

        mCurrElement = ELEMENT_UNDEFINED;
        break;
    case MASK:
        iom = static_cast<ImageObjectMask*>(mio);
        switch (mCurrElement)
        {
        case WIDTH:
            iom->mWidth = ch.toInt();
            break;
        case HEIGHT:
            iom->mHeight = ch.toInt();
            break;
        case DIAMETER:
            iom->mDiameter = ch.toInt();
            break;
        case OFFSET_X:
            iom->mOffsetX = ch.toInt();
            break;
        case OFFSET_Y:
            iom->mOffsetY = ch.toInt();
            break;
        case HOR_LINES_DIST:
            iom->mHorLinesDistance = ch.toInt();
            break;
        case VERT_LINES_DIST:
            iom->mVertLinesDistance = ch.toInt();
            break;
        case VERT_LINES_WIDTH:
            iom->mVertLinesWidth = ch.toInt();
            break;
        case VERT_LINES_PRINT_EVERY_LINE:
            iom->mVertLinesDrawEveryLine = ch.toInt();
            break;
        }

        mCurrElement = ELEMENT_UNDEFINED;
        break;
    case LINE:
        iol = static_cast<ImageObjectLine*>(mio);
        switch (mCurrElement)
        {
        case FROM_X:
            iol->mFromX = ch.toInt();
            break;
        case FROM_Y:
            iol->mFromY = ch.toInt();
            break;
        case TO_X:
            iol->mToX = ch.toInt();
            break;
        case TO_Y:
            iol->mToY = ch.toInt();
            break;
        }

        mCurrElement = ELEMENT_UNDEFINED;

        break;
    case GRID:
        iog = static_cast<ImageObjectGrid*>(mio);
        switch (mCurrElement)
        {
        case FROM_X:
            iog->mOffsetX = ch.toInt();
            break;
        case FROM_Y:
            iog->mOffsetY = ch.toInt();
            break;
        case WIDTH:
            iog->mWidth = ch.toInt();
            break;
        case HEIGHT:
            iog->mHeight = ch.toInt();
            break;
        case DIAMETER:
            iog->mDiameter = ch.toInt();
            break;
        case HOR_LINES_DIST:
            iog->mHorLinesDistance = ch.toInt();
            break;
        case VERT_LINES_DIST:
            iog->mVertLinesDistance = ch.toInt();
            break;
        case PRINT_CONTOUR:
            iog->mPrintContour = ch.toInt();
            break;
        case PRINT_HOR_LINES:
            iog->mPrintHorLines = ch.toInt();
            break;
        case PRINT_VERT_LINES:
            iog->mPrintVertLines = ch.toInt();
            break;
        }

        mCurrElement = ELEMENT_UNDEFINED;

        break;
    }

    return true;
}
