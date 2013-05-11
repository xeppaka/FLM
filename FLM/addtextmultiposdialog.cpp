#include "addtextmultiposdialog.h"
#include "ui_addtextmultiposdialog.h"
#include "falconrender.h"
#include <QFileDialog>

AddTextMultiPosDialog::AddTextMultiPosDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTextMultiPosDialog)
{
    ui->setupUi(this);
    ui->graphicsViewTextMPPicture->setScene(&gs);
}

AddTextMultiPosDialog::~AddTextMultiPosDialog()
{
    delete ui;
}

void AddTextMultiPosDialog::on_checkBoxFontInternal_toggled(bool checked)
{
    if (checked)
    {
        ui->lineEditFont->setEnabled(false);
        ui->toolButtonFontPath->setEnabled(false);
    }
    else
    {
        ui->lineEditFont->setEnabled(true);
        ui->toolButtonFontPath->setEnabled(true);
    }
}

void AddTextMultiPosDialog::on_toolButtonFontPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open font"),
                                                    tr(""),
                                                     tr("Truetype (*.ttf);;All files (*.*)"));
    if (fileName.length())
        ui->lineEditFont->setText(fileName);
}

void AddTextMultiPosDialog::on_comboWayOfDrawing_currentIndexChanged(int index)
{
    if (index == 0 || index == 2)
        ui->spinPrintEveryLine->setEnabled(true);
    else
        ui->spinPrintEveryLine->setEnabled(false);
}

void AddTextMultiPosDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectTextMultiPos();

    mResultObject->mFirstLine = ui->lineFirstLine->text();
    mResultObject->mSecondLine = ui->lineSecondLine->text();
    mResultObject->mNumberPrefix = ui->lineStartNumber_prefix->text();
    mResultObject->mNumberPostfix = ui->lineStartNumber_postfix->text();
    if (ui->checkBoxFontInternal->isChecked())
        mResultObject->mFont = INTERNAL_FONT;
    else
    {
        if (ui->lineEditFont->text().length() != 0)
            mResultObject->mFont = ui->lineEditFont->text();
        else
            mResultObject->mFont = INTERNAL_FONT;
    }
    mResultObject->mSizeStr = ui->doubleSpinSizeStr->value();
    mResultObject->mSizeNum = ui->doubleSpinSizeNum->value();
    mResultObject->mScaleXStr = ui->doubleSpinScaleXStr->value();
    mResultObject->mScaleYStr = ui->doubleSpinScaleYStr->value();
    mResultObject->mScaleXNum = ui->doubleSpinScaleXNum->value();
    mResultObject->mScaleYNum = ui->doubleSpinScaleYNum->value();
    mResultObject->mOffsetX = ui->spinOffsetX->value();
    mResultObject->mOffsetY = ui->spinOffsetY->value();
    mResultObject->mNumber = ui->spinStartNumber->value();
    mResultObject->mStep = ui->spinStep->value();
    mResultObject->mHorCount = ui->spinHorCount->value();
    mResultObject->mVertCount = ui->spinVertCount->value();
    mResultObject->mHorInterval = ui->spinHorInterval->value();
    mResultObject->mVertInterval = ui->spinVertInterval->value();
    mResultObject->mLinesDistance = ui->spinLinesDistance->value();
    mResultObject->mRadius = ui->spinRadius->value();
    mResultObject->mAdditionalLetterSpace = ui->spinAdditionalLetterSpace->value();

    switch (ui->comboWayOfDrawing->currentIndex())
    {
    case 0:
        mResultObject->mRenderTextWay = RENDER_INLINE;
        mResultObject->mPrintEveryLine = ui->spinPrintEveryLine->value();
        break;
    case 1:
        mResultObject->mRenderTextWay = RENDER_OUTLINE;
        mResultObject->mPrintEveryLine = 30;
        break;
    case 2:
        mResultObject->mRenderTextWay = RENDER_BOTH;
        mResultObject->mPrintEveryLine = ui->spinPrintEveryLine->value();
        break;
    }
    // picture
    mResultObject->mPictureDrawDistance = ui->spinTextMPTextDistance->value();
    mResultObject->mPicture.mDrawEveryLine = ui->spinTextMPPictureDrawEveryLine->value();
    switch (ui->comboTextMPPicturePos->currentIndex()) {
    case 0:
        mResultObject->mPicturePosition = TOP;
        break;
    case 1:
        mResultObject->mPicturePosition = RIGHT;
        break;
    case 2:
        mResultObject->mPicturePosition = BOTTOM;
        break;
    case 3:
        mResultObject->mPicturePosition = LEFT;
        break;
    }
    if (ui->radioTextMPPictureScale->isChecked()) {
        mResultObject->mPicture.mActiveSize = SIZE_SCALED;
        mResultObject->mPicture.mScaleX = ui->doubleTextMPPictureScaleX->value();
        mResultObject->mPicture.mScaleY = ui->doubleTextMPPictureScaleY->value();
    } else {
        mResultObject->mPicture.mActiveSize = SIZE_FIXED;
        mResultObject->mPicture.mWidth = ui->spinTextMPPictureWidth->value();
        mResultObject->mPicture.mHeight = ui->spinTextMPPictureHeight->value();
    }
    mResultObject->mPicture.mImage = image;
    done(1);
}

void AddTextMultiPosDialog::on_buttonBox_rejected()
{
    done(0);
}

void AddTextMultiPosDialog::on_buttonPictureLoad_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open bitmap"),
                                                    tr(""),
                                                     tr("Image file (*.bmp *.jpg *.jpeg *.png)"));
    if (fileName.length())
        ui->lineTextMPBitmapPath->setText(fileName);
    if (image.load(fileName)) {
        gs.clear();
        QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->graphicsViewTextMPPicture->width(),
                                                          ui->graphicsViewTextMPPicture->height(),
                                                          Qt::KeepAspectRatio);
        gs.addPixmap(pixmap);
    }
}
