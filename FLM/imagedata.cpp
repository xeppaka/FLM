#include "imagedata.h"

ImageData::ImageData()
{

}

ImageData::~ImageData()
{
    free();
}

int ImageData::length()
{
    return mData.size();
}

ImageObject* ImageData::getObject(int index)
{
    return mData[index];
}

void ImageData::addObject(ImageObject *object)
{
    mData.push_back(object);
}

void ImageData::removeObject(int index)
{
    ImageObject *io = mData.takeAt(index);
    delete io;
}

void ImageData::free()
{
    QList<ImageObject*>::iterator i;
    for (i = mData.begin();i != mData.end();++i)
        delete *i;
    mData.clear();
}

int ImageObjectPicture::height() {
    if (this->mImage.isNull())
        return -1;

    if (this->mActiveSize == SIZE_FIXED)
        return this->mHeight;
    else
        return this->mScaleY * this->mImage.height();
}

int ImageObjectPicture::width() {
    if (this->mImage.isNull())
        return -1;

    if (this->mActiveSize == SIZE_FIXED)
        return this->mWidth;
    else
        return this->mScaleX * this->mImage.width();
}
