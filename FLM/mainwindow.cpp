#include "stdheader.h"
#include "optionsdialog.h"
#include "addtextdialog.h"
#include "addtextmultiposdialog.h"
#include "addmaskdialog.h"
#include "addbitmapdialog.h"
#include "addconfigdialog.h"
#include "addlinedialog.h"
#include "addgriddialog.h"
#include "imagedata.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "logger.h"

#include <QFileDialog>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //setup UI
    ui->setupUi(this);

    QWidget *spacerWidget;

    QHBoxLayout *repeatLayout = new QHBoxLayout();
    repeatLayout->setContentsMargins(0, 0, 0, 0);
    repeatLayout->addWidget(ui->labelRepeat);
    repeatLayout->addWidget(ui->spinRepeatCount);
    QWidget *repeatWidget = new QWidget();
    repeatWidget->setLayout(repeatLayout);
    ui->statusBar->addPermanentWidget(repeatWidget);

    spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(25);
    ui->statusBar->addPermanentWidget(spacerWidget);

    QHBoxLayout *configLayout = new QHBoxLayout();
    configLayout->setContentsMargins(0, 0, 0, 0);
    configLayout->addWidget(ui->labelConfig);
    configLayout->addWidget(ui->comboConfig);
    configLayout->addWidget(ui->buttonNewConfig);
    QWidget *configWidget = new QWidget();
    configWidget->setLayout(configLayout);
    ui->statusBar->addPermanentWidget(configWidget);

    spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(25);
    ui->statusBar->addPermanentWidget(spacerWidget);

    ui->statusBar->addPermanentWidget(ui->statusCorrectionsReloadButton);
    ui->statusBar->addPermanentWidget(ui->statusCorrectionsLabel);
    ui->statusBar->addPermanentWidget(ui->statusDeviceLabel);

    ui->toolBar->addWidget(ui->buttonReset);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(3);
    ui->toolBar->addWidget(spacerWidget);

    ui->toolBar->addWidget(ui->buttonStart);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(3);
    ui->toolBar->addWidget(spacerWidget);

    ui->toolBar->addWidget(ui->buttonStop);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(7);
    ui->toolBar->addWidget(spacerWidget);
    ui->toolBar->addSeparator();
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(7);
    ui->toolBar->addWidget(spacerWidget);

    ui->toolBar->addWidget(ui->buttonLoad);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(3);
    ui->toolBar->addWidget(spacerWidget);

    ui->toolBar->addWidget(ui->buttonSave);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(3);
    ui->toolBar->addWidget(spacerWidget);

    ui->toolBar->addWidget(ui->buttonSaveAs);
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(7);
    ui->toolBar->addWidget(spacerWidget);
    ui->toolBar->addSeparator();
    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spacerWidget->setMinimumWidth(7);
    ui->toolBar->addWidget(spacerWidget);
    ui->toolBar->addWidget(ui->buttonOptions);

    spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    ui->toolBar->addWidget(spacerWidget);
    ui->toolBar->addWidget(ui->progressBarJobStatus);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(ui->lcdNumberJobTime);
    ui->toolBar->addSeparator();

    //create managers for laser and graphics
    mGOD = new GraphicsOutputDevice(ui->graphicsView);
    mFalconRender = new FalconRender();
    mImageData = new ImageData();
    mTimer = new QTimer();
    connect(mTimer,SIGNAL(timeout()),this,SLOT(on_Timer_timeout()));

    mFLMSettings = new FLMSettings();
    mFLMSettings->load();
    this->updateComboConfigs();
    settingsToUi();

    ui->frameButtons->setVisible(false);
    ui->scrollAreaText->setVisible(false);
    ui->scrollAreaTextMultiPos->setVisible(false);
    ui->scrollAreaMask->setVisible(false);
    ui->scrollAreaPicture->setVisible(false);
    ui->scrollAreaLine->setVisible(false);
    ui->scrollAreaGrid->setVisible(false);

    mFOD = new FalconOutputDevice();
    mFOD->openDevice();
    updateControls(mFOD->isDeviceReady(),&mFOD->mDeviceInfo);

    mTime = new QTime();
    mLaserThread = 0;

    mScreenMatrix = new QMatrix(1,0,0,-1,0,FIELD_HEIGHT - 1);
    mFieldMatrix = new QMatrix();
    mFieldMatrix->rotate(90);
    mFieldMatrix->scale(1,-1);

    mCurrFileName = "";
    mSaved = false;
    mXmlio = new Xmlio();

    ui->graphicsViewOnePicture->setScene(&mOnePreviewScene);
    ui->graphicsViewTextMPPicture->setScene(&mTextMPPreviewScene);

    ui->graphicsView->installEventFilter(this);
    renderImageOnScreen();

    //Logger::programStarted();
}

MainWindow::~MainWindow()
{
    mTimer->stop();
    delete mTimer;
    delete ui;
    delete mGOD;
    delete mFOD;
    delete mFalconRender;
    mImageData->free();
    delete mImageData;
    delete mFLMSettings;
    delete mXmlio;
    delete mTime;
    delete mScreenMatrix;
    delete mFieldMatrix;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Space)
            {
                on_buttonStart_clicked();
                return true;
            }
        }
        return false;
    } else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::settingsToUi()
{
    ui->sliderScale->setValue(mFLMSettings->getActiveConfig()->mScale);
    on_sliderScale_valueChanged(mFLMSettings->getActiveConfig()->mScale);
    updateCorrectionsStatus();
}

void MainWindow::uiToSettings()
{
    mFLMSettings->getActiveConfig()->mScale = ui->sliderScale->value();
}

void MainWindow::updateCorrectionsStatus()
{
    QString color, text;
    QString label = "<font color=\'%1\'>%2</font>";
    if (mFLMSettings->mCorrMatrixLoaded)
    {
        text = tr("Файл с коррекциями найден");
        color = "GREEN";
    } else
    {
        text = tr("Файл с коррекциями не найден");
        color = "RED";
    }
    ui->statusCorrectionsLabel->setText(label.arg(color).arg(text));
}

void MainWindow::updateControls(bool deviceReady, DeviceInfo *devInfo)
{
    QString color;
    QString label = "<font color=\'%1\'>%2</font>";
    QString laser;

    if ( deviceReady )
    {
        color = "GREEN";
        laser = tr("Лазер включен");
    }
    else
    {
        color = "RED";
        laser = tr("Лазер выключен");
    }
    ui->statusDeviceLabel->setText(label.arg(color).arg(laser));
    if ( deviceReady )
    {
        ui->buttonStart->setEnabled(true);
        setDeviceInformation(devInfo);
    }
    else
    {
        ui->buttonStart->setEnabled(false);
        clearDeviceInformation();
    }
    ui->buttonStop->setEnabled(false);
}

void MainWindow::setDeviceInformation(DeviceInfo *devInfo)
{
    ui->labelVersion->setText(devInfo->mVersion);
    ui->labelVersionCPU->setNum(devInfo->mCPUVersion);
    ui->labelVersionFPGA->setNum(devInfo->mFPGAVersion);
    ui->labelBufferRead->setNum(devInfo->mInBufferSize);
    ui->labelBufferWrite->setNum(devInfo->mOutBufferSize);
    ui->labelDAC->setNum(devInfo->mDAWidth);
}

void MainWindow::clearDeviceInformation()
{
    ui->labelVersion->setText("");
    ui->labelVersionCPU->setText("");
    ui->labelVersionFPGA->setText("");
    ui->labelBufferRead->setText("");
    ui->labelBufferWrite->setText("");
    ui->labelDAC->setText("");
}

void MainWindow::on_buttonStart_clicked()
{
    if (mLaserThread || !mFOD->isDeviceReady())
        return;

    if (renderImageOnLaser())
    {
        if (mFalconRender->resultInfo() == RENDER_OK_OUT_FIELD)
            if (!showImageOutWarning())
                return;

        ui->progressBarJobStatus->setValue(0);
        ui->lcdNumberJobTime->setProperty("value",QVariant(0));

        switchUiForLaserWork();

        mLaserThread = new LaserThread(mFOD);
        connect(mLaserThread,SIGNAL(finished()),this,SLOT(on_LaserThread_finished()));

        mTime->start();
        mTimer->start(TIMER_REFRESH);
        mLaserThread->start();
    } else
    {
        showErrorRenderingMessage();
    }
}

void MainWindow::on_buttonStop_clicked()
{
    if (mLaserThread)
        mLaserThread->interrupt();
}

void MainWindow::on_buttonReset_clicked()
{
    mFOD->reset();
    updateControls(mFOD->isDeviceReady(),&mFOD->mDeviceInfo);
}

void MainWindow::on_LaserThread_finished()
{    
    ui->lcdNumberJobTime->setProperty("intValue",mTime->elapsed());

    mTimer->stop();
    ui->progressBarJobStatus->setValue(0);

    delete mLaserThread;
    mLaserThread = 0;

    switchUiForIdle();

    bool needUpdate = false;
    for (int i = 0;i < mImageData->length();i++)
    {
        ImageObject* io = mImageData->getObject(i);
        switch (io->mType)
        {
        case TEXT_MULTI_POS:
            {
                ImageObjectTextMultiPos *iotmp = static_cast<ImageObjectTextMultiPos*>(io);
                int before = iotmp->mNumber;
                iotmp->mNumber += iotmp->mHorCount * iotmp->mVertCount * iotmp->mStep;
                int after = iotmp->mNumber;
                if (before / 100 < after / 100) {
                    Beep(9000, 500);
                    Sleep(1000);
                    Beep(9000, 500);
                }

                int num = ui->listObjects->currentRow();
                if (num >= 0)
                {
                    ImageObject *io = mImageData->getObject(num);
                    if (io == iotmp)
                        ui->spinStartNumber->setValue(iotmp->mNumber);
                }

                needUpdate = true;
            }
            break;
        default:
            break;
        }
    }

    if (needUpdate)
        renderImageOnScreen();

//    if ((mFOD->getCounter() <= 20) || (mFOD->getCounter() % 10 == 0))
//        Logger::updateCounter(mFOD->getCounter());
}

void MainWindow::on_buttonOptions_clicked()
{
    OptionsDialog optitonsDialog(mFLMSettings,this);
    if (optitonsDialog.exec() == QDialog::Accepted)
    {
        mFLMSettings->save(ui->comboConfig->currentIndex());
        renderImageOnScreen();
    }
}

bool MainWindow::renderImageOnScreen()
{
    return renderImageOnScreen(mFLMSettings->getActiveConfig()->mRenderMode);
}

bool MainWindow::renderImageOnScreen(Render_Mode mode)
{
    int scrollPosX = ui->graphicsView->verticalScrollBar()->value();
    int scrollPosY = ui->graphicsView->horizontalScrollBar()->value();

    mGOD->setRenderMode(mode, mFLMSettings->getActiveConfig()->mDrawIdleMotion);
    if (mFalconRender->begin(mGOD, mFLMSettings, mScreenMatrix, mode == FAST))
        if (mFalconRender->renderImage(mImageData, 1))
            if (mFalconRender->end())
            {
                ui->graphicsView->verticalScrollBar()->setValue(scrollPosX);
                ui->graphicsView->horizontalScrollBar()->setValue(scrollPosY);
                return true;
            }
    return false;
}

bool MainWindow::renderImageOnLaser()
{
    int repeatCount = ui->spinRepeatCount->value();

    if (mFalconRender->begin(mFOD, mFLMSettings, mFieldMatrix))
        if (mFalconRender->renderImage(mImageData, repeatCount))
            if (mFalconRender->end())
                return true;
    return false;
}

void MainWindow::on_listObjects_itemClicked(QListWidgetItem* item)
{
    int num = ui->listObjects->currentRow();
    ImageObject *io = mImageData->getObject(num);
    ImageObjectTextMultiPos *iotmp;
    ImageObjectText *iot;
    ImageObjectMask *iom;
    ImageObjectPicture *iop;
    ImageObjectLine *iol;
    ImageObjectGrid *iog;

    ui->scrollAreaText->setVisible(false);
    ui->scrollAreaTextMultiPos->setVisible(false);
    ui->scrollAreaMask->setVisible(false);
    ui->scrollAreaPicture->setVisible(false);
    ui->scrollAreaLine->setVisible(false);
    ui->scrollAreaGrid->setVisible(false);

    switch (io->mType)
    {
    case TEXT:
        ui->scrollAreaText->setVisible(true);
        iot = static_cast<ImageObjectText*>(io);
        ui->lineEditText->setText(iot->mText);
        if (iot->mFont.compare(INTERNAL_FONT,Qt::CaseSensitive) == 0)
        {
            ui->lineEditFont->setText("");
            ui->lineEditFont->setEnabled(false);
            ui->toolButtonFontPath->setEnabled(false);
            ui->checkBoxFontInternal->setChecked(true);
        } else
        {
            ui->lineEditFont->setEnabled(true);
            ui->toolButtonFontPath->setEnabled(true);
            ui->checkBoxFontInternal->setChecked(false);
            ui->lineEditFont->setText(iot->mFont);
        }
        ui->doubleSpinBoxTextSize->setValue(iot->mSize);
        ui->spinTextX->setValue(iot->mOffsetX);
        ui->spinTextY->setValue(iot->mOffsetY);
        ui->spinRadiusText->setValue(iot->mRadius);
        ui->spinAdditionalLetterSpaceText->setValue(iot->mAdditionalLetterSpace);
        switch (iot->mRenderTextWay)
        {
        case RENDER_INLINE:
            ui->comboWayOfDrawing->setCurrentIndex(0);
            ui->spinPrintEveryLine->setEnabled(true);
            ui->spinPrintEveryLine->setValue(iot->mPrintEveryLine);
            break;
        case RENDER_OUTLINE:
            ui->comboWayOfDrawing->setCurrentIndex(1);
            ui->spinPrintEveryLine->setEnabled(false);
            ui->spinPrintEveryLine->setValue(1);
            break;
        case RENDER_BOTH:
            ui->comboWayOfDrawing->setCurrentIndex(2);
            ui->spinPrintEveryLine->setEnabled(true);
            ui->spinPrintEveryLine->setValue(iot->mPrintEveryLine);
            break;
        }
        break;
    case TEXT_MULTI_POS:
        ui->scrollAreaTextMultiPos->setVisible(true);
        iotmp = static_cast<ImageObjectTextMultiPos*>(io);
        ui->lineFirstLine->setText(iotmp->mFirstLine);
        ui->lineSecondLine->setText(iotmp->mSecondLine);
        ui->lineStartNumber_prefix->setText(iotmp->mNumberPrefix);
        ui->lineStartNumber_postfix->setText(iotmp->mNumberPostfix);
        ui->spinStartNumber->setValue(iotmp->mNumber);
        ui->spinStep->setValue(iotmp->mStep);
        if (iotmp->mFont.compare(INTERNAL_FONT,Qt::CaseSensitive) == 0)
        {
            ui->lineEditFont_2->setText("");
            ui->lineEditFont_2->setEnabled(false);
            ui->toolButtonFontPath_2->setEnabled(false);
            ui->checkBoxFontInternal_2->setChecked(true);
        } else
        {
            ui->lineEditFont_2->setEnabled(true);
            ui->toolButtonFontPath_2->setEnabled(true);
            ui->checkBoxFontInternal_2->setChecked(false);
            ui->lineEditFont_2->setText(iotmp->mFont);
        }
        ui->spinOffsetX->setValue(iotmp->mOffsetX);
        ui->spinOffsetY->setValue(iotmp->mOffsetY);
        ui->spinHorCount->setValue(iotmp->mHorCount);
        ui->spinVertCount->setValue(iotmp->mVertCount);
        ui->spinHorInterval->setValue(iotmp->mHorInterval);
        ui->spinVertInterval->setValue(iotmp->mVertInterval);
        ui->doubleSpinSizeStr->setValue(iotmp->mSizeStr);
        ui->doubleSpinSizeNum->setValue(iotmp->mSizeNum);
        ui->doubleSpinScaleXStr->setValue(iotmp->mScaleXStr);
        ui->doubleSpinScaleYStr->setValue(iotmp->mScaleYStr);
        ui->doubleSpinScaleXNum->setValue(iotmp->mScaleXNum);
        ui->doubleSpinScaleYNum->setValue(iotmp->mScaleYNum);
        ui->spinLinesDistance->setValue(iotmp->mLinesDistance);
        ui->spinPrintEveryLine_2->setValue(iotmp->mPrintEveryLine);
        ui->spinRadiusTextMultiPos->setValue(iotmp->mRadius);
        ui->spinAdditionalLetterSpaceTextMultiPos->setValue(iotmp->mAdditionalLetterSpace);
        switch (iotmp->mRenderTextWay)
        {
        case RENDER_INLINE:
            ui->comboWayOfDrawing_2->setCurrentIndex(0);
            ui->spinPrintEveryLine_2->setEnabled(true);
            break;
        case RENDER_OUTLINE:
            ui->comboWayOfDrawing_2->setCurrentIndex(1);
            ui->spinPrintEveryLine_2->setEnabled(false);
            break;
        case RENDER_BOTH:
            ui->comboWayOfDrawing_2->setCurrentIndex(2);
            ui->spinPrintEveryLine_2->setEnabled(true);
            break;
        }
        ui->spinPrintEveryLine_2->setValue(iotmp->mPrintEveryLine);
        // picture
        switch (iotmp->mPicturePosition) {
        case TOP:
            ui->comboTextMPPicturePos->setCurrentIndex(0);
            break;
        case RIGHT:
            ui->comboTextMPPicturePos->setCurrentIndex(1);
            break;
        case BOTTOM:
            ui->comboTextMPPicturePos->setCurrentIndex(2);
            break;
        case LEFT:
            ui->comboTextMPPicturePos->setCurrentIndex(3);
            break;
        }
        on_radioTextMPPictureScale_toggled(iotmp->mPicture.mActiveSize == SIZE_SCALED);
        on_radioTextMPPictureFixed_toggled(iotmp->mPicture.mActiveSize == SIZE_FIXED);
        if (iotmp->mPicture.mActiveSize == SIZE_SCALED) {
            ui->doubleTextMPPictureScaleX->setValue(iotmp->mPicture.mScaleX);
            ui->doubleTextMPPictureScaleY->setValue(iotmp->mPicture.mScaleY);
        } else {
            ui->spinTextMPPictureWidth->setValue(iotmp->mPicture.mWidth);
            ui->spinTextMPPictureHeight->setValue(iotmp->mPicture.mHeight);
        }
        ui->spinTextMPPictureDrawEveryLine->setValue(iotmp->mPicture.mDrawEveryLine);
        ui->spinTextMPTextDistance->setValue(iotmp->mPictureDrawDistance);
        if (!iotmp->mPicture.mImage.isNull()) {
            mTextMPPreviewScene.clear();
            QPixmap pixmap = QPixmap::fromImage(iotmp->mPicture.mImage).scaled(ui->graphicsViewTextMPPicture->width(),
                                                              ui->graphicsViewTextMPPicture->height(),
                                                              Qt::KeepAspectRatio);
            mTextMPPreviewScene.addPixmap(pixmap);
            mTextMPPreviewScene.setSceneRect(0, 0, pixmap.width(), pixmap.height());
            mTextMPPicturePreview = iotmp->mPicture.mImage;
        }

        break;
    case MASK:
        ui->scrollAreaMask->setVisible(true);
        iom = static_cast<ImageObjectMask*>(io);
        ui->spinMaskOffsetX->setValue(iom->mOffsetX);
        ui->spinMaskOffsetY->setValue(iom->mOffsetY);
        ui->spinWidth->setValue(iom->mWidth);
        ui->spinHeight->setValue(iom->mHeight);
        ui->spinDiameter->setValue(iom->mDiameter);
        ui->spinHorLinesDistance->setValue(iom->mHorLinesDistance);
        ui->spinVertLinesDistance->setValue(iom->mVertLinesDistance);
        ui->spinVertLinesWidth->setValue(iom->mVertLinesWidth);
        ui->spinVertLinesDrawEveryLine->setValue(iom->mVertLinesDrawEveryLine);
        break;
    case PICTURE:
        ui->scrollAreaPicture->setVisible(true);
        iop = static_cast<ImageObjectPicture*>(io);
        this->on_radioPictureScale_toggled(iop->mActiveSize == SIZE_SCALED);
        this->on_radioPictureFixed_toggled(iop->mActiveSize == SIZE_FIXED);
        if (iop->mActiveSize == SIZE_SCALED) {
            ui->doublePictureScaleX->setValue(iop->mScaleX);
            ui->doublePictureScaleY->setValue(iop->mScaleY);
        } else {
            ui->spinPictureWidth->setValue(iop->mWidth);
            ui->spinPictureHeight->setValue(iop->mHeight);
        }
        ui->spinPictureOffsetX->setValue(iop->mShiftX);
        ui->spinPictureOffsetY->setValue(iop->mShiftY);
        ui->spinPictureDrawEveryLine->setValue(iop->mDrawEveryLine);
        mOnePreviewScene.clear();
        if (!iop->mImage.isNull()) {
            QPixmap pixmap = QPixmap::fromImage(iop->mImage).scaled(ui->graphicsViewOnePicture->width(),
                                                              ui->graphicsViewOnePicture->height(),
                                                              Qt::KeepAspectRatio);
            mOnePreviewScene.addPixmap(pixmap);
            mOnePreviewScene.setSceneRect(0, 0, pixmap.width(), pixmap.height());
            mOnePicturePreview = iop->mImage;
        }
        break;
    case LINE:
        iol = static_cast<ImageObjectLine*>(io);
        ui->scrollAreaLine->setVisible(true);
        ui->spinLineStartX->setValue(iol->mFromX);
        ui->spinLineStartY->setValue(iol->mFromY);
        ui->spinLineEndX->setValue(iol->mToX);
        ui->spinLineEndY->setValue(iol->mToY);
        break;
    case GRID:
        iog = static_cast<ImageObjectGrid*>(io);
        ui->scrollAreaGrid->setVisible(true);
        ui->spinGridOffsetX->setValue(iog->mOffsetX);
        ui->spinGridOffsetY->setValue(iog->mOffsetY);
        ui->spinGridWidth->setValue(iog->mWidth);
        ui->spinGridHeight->setValue(iog->mHeight);
        ui->spinGridDiameter->setValue(iog->mDiameter);
        ui->spinGridHorLinesDistance->setValue(iog->mHorLinesDistance);
        ui->spinGridVerLinesDistance->setValue(iog->mVertLinesDistance);
        ui->checkGridPrintContour->setChecked(iog->mPrintContour);
        ui->checkGridPrintHorLines->setChecked(iog->mPrintHorLines);
        ui->checkGridPrintVerLines->setChecked(iog->mPrintVertLines);
        break;
    }
    ui->frameButtons->setVisible(true);
}

void MainWindow::on_buttonSaveImageObject_clicked()
{
    int num = ui->listObjects->currentRow();
    if (num == -1)
        return;

    ImageObject *io = mImageData->getObject(num);
    ImageObjectTextMultiPos *iotmp;
    ImageObjectText *iot;
    ImageObjectMask *iom;
    ImageObjectPicture *iop;
    ImageObjectLine *iol;
    ImageObjectGrid *iog;

    switch (io->mType)
    {
    case TEXT:
        iot = static_cast<ImageObjectText*>(io);
        iot->mText = ui->lineEditText->text();
        if (ui->checkBoxFontInternal->isChecked())
            iot->mFont = INTERNAL_FONT;
        else
        {
            if (ui->lineEditFont->text().length() != 0)
                iot->mFont = ui->lineEditFont->text();
            else
            {
                iot->mFont = INTERNAL_FONT;
                ui->checkBoxFontInternal->setChecked(true);
                ui->lineEditFont->setEnabled(false);
                ui->toolButtonFontPath->setEnabled(false);
            }
        }
        iot->mOffsetX = ui->spinTextX->value();
        iot->mOffsetY = ui->spinTextY->value();
        iot->mSize = ui->doubleSpinBoxTextSize->value();
        iot->mRadius = ui->spinRadiusText->value();
        iot->mAdditionalLetterSpace = ui->spinAdditionalLetterSpaceText->value();

        switch (ui->comboWayOfDrawing->currentIndex())
        {
        case 0:
            iot->mRenderTextWay = RENDER_INLINE;
            iot->mPrintEveryLine = ui->spinPrintEveryLine->value();
            break;
        case 1:
            iot->mRenderTextWay = RENDER_OUTLINE;
            break;
        case 2:
            iot->mRenderTextWay = RENDER_BOTH;
            iot->mPrintEveryLine = ui->spinPrintEveryLine->value();
            break;
        }

        //update listObjectItem. may be text was changed.
        ui->listObjects->item(num)->setText(tr("TEXT: ") + iot->mText);
        break;
    case TEXT_MULTI_POS:
        iotmp = static_cast<ImageObjectTextMultiPos*>(io);
        iotmp->mFirstLine = ui->lineFirstLine->text();
        iotmp->mSecondLine = ui->lineSecondLine->text();
        iotmp->mNumberPrefix = ui->lineStartNumber_prefix->text();
        iotmp->mNumberPostfix = ui->lineStartNumber_postfix->text();
        iotmp->mNumber = ui->spinStartNumber->value();
        iotmp->mStep = ui->spinStep->value();
        if (ui->checkBoxFontInternal_2->isChecked())
            iotmp->mFont = INTERNAL_FONT;
        else
        {
            if (ui->lineEditFont_2->text().length() != 0)
                iotmp->mFont = ui->lineEditFont_2->text();
            else
            {
                iotmp->mFont = INTERNAL_FONT;
                ui->checkBoxFontInternal_2->setChecked(true);
                ui->lineEditFont_2->setEnabled(false);
                ui->toolButtonFontPath_2->setEnabled(false);
            }
        }
        iotmp->mOffsetX = ui->spinOffsetX->value();
        iotmp->mOffsetY = ui->spinOffsetY->value();
        iotmp->mHorCount = ui->spinHorCount->value();
        iotmp->mVertCount = ui->spinVertCount->value();
        iotmp->mHorInterval = ui->spinHorInterval->value();
        iotmp->mVertInterval = ui->spinVertInterval->value();
        iotmp->mSizeStr = ui->doubleSpinSizeStr->value();
        iotmp->mSizeNum = ui->doubleSpinSizeNum->value();
        iotmp->mScaleXStr = ui->doubleSpinScaleXStr->value();
        iotmp->mScaleYStr = ui->doubleSpinScaleYStr->value();
        iotmp->mScaleXNum = ui->doubleSpinScaleXNum->value();
        iotmp->mScaleYNum = ui->doubleSpinScaleYNum->value();
        iotmp->mLinesDistance = ui->spinLinesDistance->value();
        iotmp->mPrintEveryLine = ui->spinPrintEveryLine->value();
        iotmp->mRadius = ui->spinRadiusTextMultiPos->value();
        iotmp->mAdditionalLetterSpace = ui->spinAdditionalLetterSpaceTextMultiPos->value();

        switch (ui->comboWayOfDrawing_2->currentIndex())
        {
        case 0:
            iotmp->mRenderTextWay = RENDER_INLINE;
            iotmp->mPrintEveryLine = ui->spinPrintEveryLine_2->value();
            break;
        case 1:
            iotmp->mRenderTextWay = RENDER_OUTLINE;
            break;
        case 2:
            iotmp->mRenderTextWay = RENDER_BOTH;
            iotmp->mPrintEveryLine = ui->spinPrintEveryLine_2->value();
            break;
        }
        ui->listObjects->item(num)->setText(QString(tr("TEXT MULT: %1:%2")).arg(iotmp->mHorCount).arg(iotmp->mVertCount));
        // picture
        switch (ui->comboTextMPPicturePos->currentIndex()) {
        case 0:
            iotmp->mPicturePosition = TOP;
            break;
        case 1:
            iotmp->mPicturePosition = RIGHT;
            break;
        case 2:
            iotmp->mPicturePosition = BOTTOM;
            break;
        case 3:
            iotmp->mPicturePosition = LEFT;
            break;
        }
        if (ui->radioTextMPPictureScale->isChecked()) {
            iotmp->mPicture.mActiveSize = SIZE_SCALED;
            iotmp->mPicture.mScaleX = ui->doubleTextMPPictureScaleX->value();
            iotmp->mPicture.mScaleY = ui->doubleTextMPPictureScaleY->value();
        } else {
            iotmp->mPicture.mActiveSize = SIZE_FIXED;
            iotmp->mPicture.mWidth = ui->spinTextMPPictureWidth->value();
            iotmp->mPicture.mHeight = ui->spinTextMPPictureHeight->value();
        }
        iotmp->mPicture.mDrawEveryLine = ui->spinTextMPPictureDrawEveryLine->value();
        iotmp->mPictureDrawDistance = ui->spinTextMPTextDistance->value();
        iotmp->mPicture.mImage = this->mTextMPPicturePreview;
        break;
    case MASK:
        iom = static_cast<ImageObjectMask*>(io);
        iom->mOffsetX = ui->spinMaskOffsetX->value();
        iom->mOffsetY = ui->spinMaskOffsetY->value();
        iom->mWidth = ui->spinWidth->value();
        iom->mHeight = ui->spinHeight->value();
        iom->mDiameter = ui->spinDiameter->value();
        iom->mHorLinesDistance = ui->spinHorLinesDistance->value();
        iom->mVertLinesDistance = ui->spinVertLinesDistance->value();
        iom->mVertLinesWidth = ui->spinVertLinesWidth->value();
        iom->mVertLinesDrawEveryLine = ui->spinVertLinesDrawEveryLine->value();
        break;
    case PICTURE:
        iop = static_cast<ImageObjectPicture*>(io);
        if (ui->radioPictureScale->isChecked()) {
            iop->mActiveSize = SIZE_SCALED;
            iop->mScaleX = ui->doublePictureScaleX->value();
            iop->mScaleY = ui->doublePictureScaleY->value();
        } else {
            iop->mActiveSize = SIZE_FIXED;
            iop->mWidth = ui->spinPictureWidth->value();
            iop->mHeight = ui->spinPictureHeight->value();
        }
        iop->mShiftX = ui->spinPictureOffsetX->value();
        iop->mShiftY = ui->spinPictureOffsetY->value();
        iop->mDrawEveryLine = ui->spinPictureDrawEveryLine->value();
        iop->mImage = this->mOnePicturePreview;
        break;
    case LINE:
        iol = static_cast<ImageObjectLine*>(io);
        iol->mFromX = ui->spinLineStartX->value();
        iol->mFromY = ui->spinLineStartY->value();
        iol->mToX = ui->spinLineEndX->value();
        iol->mToY = ui->spinLineEndY->value();
        break;
    case GRID:
        iog = static_cast<ImageObjectGrid*>(io);
        iog->mDiameter = ui->spinGridDiameter->value();
        iog->mOffsetX = ui->spinGridOffsetX->value();
        iog->mOffsetY = ui->spinGridOffsetY->value();
        iog->mWidth = ui->spinGridWidth->value();
        iog->mHeight = ui->spinGridHeight->value();
        iog->mHorLinesDistance = ui->spinGridHorLinesDistance->value();
        iog->mVertLinesDistance = ui->spinGridVerLinesDistance->value();
        iog->mPrintContour = ui->checkGridPrintContour->isChecked();
        iog->mPrintHorLines = ui->checkGridPrintHorLines->isChecked();
        iog->mPrintVertLines = ui->checkGridPrintVerLines->isChecked();
        break;
    }
    mSaved = false;
    updateCurrFileName();
    renderImageOnScreen();
}

void MainWindow::on_buttonRemoveImageObject_clicked()
{
    int num = ui->listObjects->currentRow();
    if (num == -1)
        return;

    mImageData->removeObject(num);
    QListWidgetItem *item = ui->listObjects->takeItem(num);
    delete item;
    item = ui->listObjects->currentItem();
    if (item != 0)
        on_listObjects_itemClicked(item);
    else
    {
        ui->scrollAreaMask->setVisible(false);
        ui->scrollAreaText->setVisible(false);
        ui->scrollAreaTextMultiPos->setVisible(false);
        ui->scrollAreaPicture->setVisible(false);
        ui->scrollAreaGrid->setVisible(false);
        ui->frameButtons->setVisible(false);
    }

    mSaved = false;
    updateCurrFileName();
    renderImageOnScreen();
}

void MainWindow::on_buttonAddText_clicked()
{
    AddTextDialog atd(this);
    int result = atd.exec();
    if (result)
    {
        mImageData->addObject(atd.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = tr("TEXT: %1").arg(atd.mResultObject->mText);
        item = new QListWidgetItem(name,ui->listObjects,TEXT);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_buttonAddMask_clicked()
{
    AddMaskDialog amd(this);
    int result = amd.exec();
    if (result)
    {
        mImageData->addObject(amd.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = tr("MASK: %1x%2").arg(amd.mResultObject->mWidth).arg(amd.mResultObject->mHeight);
        item = new QListWidgetItem(name,ui->listObjects,MASK);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_buttonAddTextMultiPos_clicked()
{
    AddTextMultiPosDialog atd(this);
    int result = atd.exec();
    if (result)
    {
        mImageData->addObject(atd.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = QString(tr("TEXT MULT: %1x%2")).arg(atd.mResultObject->mHorCount).arg(atd.mResultObject->mVertCount);
        item = new QListWidgetItem(name, ui->listObjects, TEXT_MULTI_POS);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_buttonAddBitmap_clicked()
{
    AddBitmapDialog abd(this);
    int result = abd.exec();
    if (result) {
        mImageData->addObject(abd.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = QString(tr("PICTURE"));
        item = new QListWidgetItem(name,ui->listObjects,PICTURE);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_buttonAddLine_clicked()
{
    AddLineDialog ald(this);
    int result = ald.exec();
    if (result) {
        mImageData->addObject(ald.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = QString(tr("LINE X0: %1, Y0: %2")).arg(ald.mResultObject->mFromX).arg(ald.mResultObject->mFromY);
        item = new QListWidgetItem(name, ui->listObjects, LINE);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_buttonAddGrid_clicked() {
    AddGridDialog agd(this);
    int result = agd.exec();
    if (result) {
        mImageData->addObject(agd.mResultObject);

        QString name;
        QListWidgetItem *item;
        name = QString(tr("GRID %1x%2")).arg(agd.mResultObject->mWidth).arg(agd.mResultObject->mHeight);
        item = new QListWidgetItem(name, ui->listObjects, GRID);
        mSaved = false;
        updateCurrFileName();
        renderImageOnScreen();
    }
}

void MainWindow::on_toolButtonFontPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open font"),
                                                    tr(""),
                                                     tr("Truetype (*.ttf)"));
    if (fileName.length())
        ui->lineEditFont->setText(fileName);
}

void MainWindow::on_checkBoxFontInternal_toggled(bool checked)
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

void MainWindow::on_Timer_timeout()
{
    ui->lcdNumberJobTime->setProperty("intValue",QVariant(mTime->elapsed()));
    ui->progressBarJobStatus->setValue(mFOD->mPercentDone);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mLaserThread)
        event->ignore();
    else {
        uiToSettings();
        mFLMSettings->save(ui->comboConfig->currentIndex());
        mFLMSettings->saveLastUsedConfig();
        //Logger::programFinished(mFOD->getCounter());
        event->accept();
    }
}

void MainWindow::switchUiForLaserWork()
{
    ui->buttonReset->setEnabled(false);
    ui->buttonStart->setEnabled(false);
    ui->buttonStop->setEnabled(true);
}

void MainWindow::switchUiForIdle()
{
    ui->buttonReset->setEnabled(true);
    ui->buttonStart->setEnabled(true);
    ui->buttonStop->setEnabled(false);
}

void MainWindow::showErrorRenderingMessage()
{
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Critical);
    mb.setWindowTitle(tr("Ошибка"));
    mb.setText(tr("Произошла ошибка при рендеринге изображения на устройство"));
    mb.exec();
}

bool MainWindow::showImageOutWarning()
{
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("К Вам вопрос"));
    mb.setText(tr("Изображение выходит за границы работы лазера и будет обрезано. Продолжить?"));    
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mb.setDefaultButton(QMessageBox::Yes);
    int result = mb.exec();
    return (result == QMessageBox::Yes);
}

void MainWindow::on_sliderScale_valueChanged(int value)
{
    ui->graphicsView->resetMatrix();
    if (value != 100)
    {
        double expValue = exp(value*SCALE_FORMULA_CONST);
        ui->graphicsView->scale(expValue*GRAPHICS_SCALE,expValue*GRAPHICS_SCALE);
    }
    ui->graphicsView->update();
    ui->scalePercentage->setText(QString("%1%").arg(value));
}

void MainWindow::on_comboWayOfDrawing_currentIndexChanged(int index)
{
    if (index == 0 || index == 2)
        ui->spinPrintEveryLine->setEnabled(true);
    else
        ui->spinPrintEveryLine->setEnabled(false);
}


void MainWindow::on_buttonShowTabWidget_clicked()
{
    if (ui->tabWidget->isVisible())
    {
        ui->tabWidget->setVisible(false);
        ui->buttonShowTabWidget->setText("<\n<\n<");
    } else
    {
        ui->tabWidget->setVisible(true);
        ui->buttonShowTabWidget->setText(">\n>\n>");
    }
}

void MainWindow::on_comboWayOfDrawing_2_currentIndexChanged(int index)
{
    if (index == 0 || index == 2)
        ui->spinPrintEveryLine_2->setEnabled(true);
    else
        ui->spinPrintEveryLine_2->setEnabled(false);
}

void MainWindow::on_checkBoxFontInternal_2_toggled(bool checked)
{
    if (checked)
    {
        ui->lineEditFont_2->setEnabled(false);
        ui->toolButtonFontPath_2->setEnabled(false);
    }
    else
    {
        ui->lineEditFont_2->setEnabled(true);
        ui->toolButtonFontPath_2->setEnabled(true);
    }
}

void MainWindow::on_toolButtonFontPath_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open font"),
                                                    tr(""),
                                                     tr("Truetype (*.ttf)"));
    if (fileName.length())
        ui->lineEditFont_2->setText(fileName);
}

void MainWindow::on_buttonMakeCenter_clicked()
{
    int num = ui->listObjects->currentRow();
    if (num == -1)
        return;

    ImageObject *io = mImageData->getObject(num);
    //ImageObjectTextMultiPos *iotmp;
    //ImageObjectText *iot;
    ImageObjectMask *iom;
    ImageObjectGrid *iog;
    switch (io->mType)
    {
    case TEXT:
        break;
    case TEXT_MULTI_POS:
        break;
    case MASK:
        iom = static_cast<ImageObjectMask*>(io);
        ui->spinMaskOffsetX->setValue(static_cast<int>(round(FIELD_WIDTH / 2.0 - iom->mWidth / 2.0)));
        ui->spinMaskOffsetY->setValue(static_cast<int>(round(FIELD_HEIGHT / 2.0 - iom->mHeight / 2.0)));
        on_buttonSaveImageObject_clicked();
        break;
    case GRID:
        iog = static_cast<ImageObjectGrid*>(io);
        ui->spinGridOffsetX->setValue(static_cast<int>(round(FIELD_WIDTH / 2.0 - iog->mWidth / 2.0)));
        ui->spinGridOffsetY->setValue(static_cast<int>(round(FIELD_HEIGHT / 2.0 - iog->mHeight / 2.0)));
        on_buttonSaveImageObject_clicked();
        break;
    }
}

void MainWindow::updateListWidget()
{
    ui->scrollAreaMask->setVisible(false);
    ui->scrollAreaPicture->setVisible(false);
    ui->scrollAreaText->setVisible(false);
    ui->scrollAreaTextMultiPos->setVisible(false);
    ui->frameButtons->setVisible(false);

    ui->listObjects->clear();
    QString name;

    for (int i = 0;i < mImageData->length();i++)
    {
        ImageObject* io = mImageData->getObject(i);
        ImageObjectTextMultiPos *iotmp;
        ImageObjectText *iot;
        ImageObjectMask *iom;
        ImageObjectLine *iol;
        ImageObjectGrid *iog;

        switch (io->mType)
        {
        case TEXT:
            iot = static_cast<ImageObjectText*>(io);
            name = tr("TEXT: %1").arg(iot->mText);
            new QListWidgetItem(name,ui->listObjects, TEXT);
            break;
        case TEXT_MULTI_POS:
            iotmp = static_cast<ImageObjectTextMultiPos*>(io);
            name = QString(tr("TEXT MULT: %1x%2")).arg(iotmp->mHorCount).arg(iotmp->mVertCount);
            new QListWidgetItem(name,ui->listObjects, TEXT_MULTI_POS);
            break;
        case MASK:
            iom = static_cast<ImageObjectMask*>(io);
            name = tr("MASK: %1x%2").arg(iom->mWidth).arg(iom->mHeight);
            new QListWidgetItem(name,ui->listObjects, MASK);
            break;
        case LINE:
            iol = static_cast<ImageObjectLine*>(io);
            name = tr("LINE: X0: %1, Y0: %2").arg(iol->mFromX).arg(iol->mFromY);
            new QListWidgetItem(name,ui->listObjects, LINE);
            break;
        case GRID:
            iog = static_cast<ImageObjectGrid*>(io);
            name = tr("GRID: %1x%2").arg(iog->mWidth).arg(iog->mHeight);
            new QListWidgetItem(name,ui->listObjects, GRID);
            break;
        }
    }
}

void MainWindow::updateComboConfigs() {
    ui->comboConfig->clear();
    int len = this->mFLMSettings->size();
    for (int i = 0; i < len; ++i) {
        RenderConfig *config = mFLMSettings->getConfig(i);
        if (config)
            ui->comboConfig->addItem(config->filename);
    }
    ui->comboConfig->setCurrentIndex(mFLMSettings->getLastUsedConfigIndex());
}

void MainWindow::updateCurrFileName()
{
    QString windowTitle = WINDOW_NAME;
    if (mCurrFileName.length())
        windowTitle.append(" ").append(mCurrFileName);

    if (!mSaved)
        windowTitle.append("*");

    setWindowTitle(windowTitle);
}

bool MainWindow::showSaveQuestionMessage()
{
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("К Вам вопрос"));
    mb.setText(tr("Текущее изображение не было сохранено. Сохранить его перед открытием нового?"));
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mb.setDefaultButton(QMessageBox::Yes);
    return mb.exec() == QMessageBox::Yes;
}

void MainWindow::showSaveErrorMessage()
{
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Critical);
    mb.setWindowTitle(tr("Ошибка"));
    mb.setText(tr("Произошла ошибка при сохранении изображения."));
    mb.exec();
}

void MainWindow::showOpenErrorMessage()
{
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Critical);
    mb.setWindowTitle(tr("Ошибка"));
    mb.setText(tr("Произошла ошибка при открытии изображения."));
    mb.exec();
}

void MainWindow::on_buttonLoad_clicked()
{
    if (mCurrFileName.length() && !mSaved && mImageData->length())
        if (showSaveQuestionMessage())
        {
            if (!mXmlio->save(mCurrFileName, mImageData))
                showSaveErrorMessage();
            else
                mSaved = true;
        }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open image"),
                                                    tr(""),
                                                     tr("FLM Xml (*.xml)"));
    if (fileName.length())
    {
        ImageData *tempImageData = mImageData;
        mImageData = new ImageData();
        if (mXmlio->load(fileName, mImageData))
        {
            mCurrFileName = fileName;
            mSaved = true;
            tempImageData->free();
            delete tempImageData;
            updateCurrFileName();
            updateListWidget();
            renderImageOnScreen();
        } else
        {
            delete mImageData;
            mImageData = tempImageData;
            showOpenErrorMessage();
        }
    } else
    {
    }
}

void MainWindow::on_buttonSave_clicked()
{
    if (!mCurrFileName.length())
        mCurrFileName = QFileDialog::getSaveFileName(this, tr("Save image"),
                                                        tr(""),
                                                         tr("FLM xml (*.xml)"));

    if (mCurrFileName.length())
    {
        if (!mXmlio->save(mCurrFileName, mImageData))
            showSaveErrorMessage();
        else
            mSaved = true;

        updateCurrFileName();
    }
}

void MainWindow::on_buttonSaveAs_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"),
                                                        tr(""),
                                                         tr("FLM xml (*.xml)"));
    if (fileName.length())
    {
        if (mXmlio->save(fileName, mImageData))
        {
            mCurrFileName = fileName;
            mSaved = true;
        }
        else
            showSaveErrorMessage();

        updateCurrFileName();
    }
}

void MainWindow::on_statusCorrectionsReloadButton_clicked()
{
    mFLMSettings->reloadCorrections();
    updateCorrectionsStatus();
}

void MainWindow::on_radioPictureScale_toggled(bool checked)
{
    ui->doublePictureScaleX->setEnabled(checked);
    ui->doublePictureScaleY->setEnabled(checked);
}

void MainWindow::on_radioPictureFixed_toggled(bool checked)
{
    ui->spinPictureWidth->setEnabled(checked);
    ui->spinPictureHeight->setEnabled(checked);
}

void MainWindow::on_buttonPictureLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open bitmap"),
                                                    tr(""),
                                                     tr("Image file (*.bmp *.jpg *.jpeg *.png)"));
    if (fileName.length())
        ui->lineBitmapPath->setText(fileName);
    if (mOnePicturePreview.load(fileName)) {
        mOnePreviewScene.clear();
        QPixmap pixmap = QPixmap::fromImage(mOnePicturePreview).scaled(ui->graphicsViewOnePicture->width(),
                                                          ui->graphicsViewOnePicture->height(),
                                                          Qt::KeepAspectRatio);
        mOnePreviewScene.addPixmap(pixmap);
    }
}

void MainWindow::on_buttonPictureClear_clicked()
{
    mOnePreviewScene.clear();
    ui->lineBitmapPath->clear();
    this->mOnePicturePreview = QImage();
}

void MainWindow::on_lineBitmapPath_returnPressed()
{
    QString fileName = ui->lineBitmapPath->text();

    if (mOnePicturePreview.load(fileName)) {
        mOnePreviewScene.clear();
        QPixmap pixmap = QPixmap::fromImage(mOnePicturePreview).scaled(ui->graphicsViewOnePicture->width(),
                                                          ui->graphicsViewOnePicture->height(),
                                                          Qt::KeepAspectRatio);
        mOnePreviewScene.addPixmap(pixmap);
    }
}

void MainWindow::on_buttonTextMPPictureLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open bitmap"),
                                                    tr(""),
                                                     tr("Image file (*.bmp *.jpg *.jpeg *.png)"));
    if (fileName.length())
        ui->lineTextMPBitmapPath->setText(fileName);
    if (mTextMPPicturePreview.load(fileName)) {
        mTextMPPreviewScene.clear();
        QPixmap pixmap = QPixmap::fromImage(mTextMPPicturePreview).scaled(ui->graphicsViewTextMPPicture->width(),
                                                          ui->graphicsViewTextMPPicture->height(),
                                                          Qt::KeepAspectRatio);
        mTextMPPreviewScene.addPixmap(pixmap);
    }
}

void MainWindow::on_lineTextMPBitmapPath_returnPressed()
{
    QString fileName = ui->lineTextMPBitmapPath->text();

    if (mTextMPPicturePreview.load(fileName)) {
        mTextMPPreviewScene.clear();
        QPixmap pixmap = QPixmap::fromImage(mTextMPPicturePreview).scaled(ui->graphicsViewTextMPPicture->width(),
                                                          ui->graphicsViewTextMPPicture->height(),
                                                          Qt::KeepAspectRatio);
        mTextMPPreviewScene.addPixmap(pixmap);
    }
}

void MainWindow::on_radioTextMPPictureScale_toggled(bool checked)
{
    ui->doubleTextMPPictureScaleX->setEnabled(checked);
    ui->doubleTextMPPictureScaleY->setEnabled(checked);
}

void MainWindow::on_radioTextMPPictureFixed_toggled(bool checked)
{
    ui->spinTextMPPictureWidth->setEnabled(checked);
    ui->spinTextMPPictureHeight->setEnabled(checked);
}

void MainWindow::on_buttonTextMPPictureClear_clicked()
{
    mTextMPPreviewScene.clear();
    ui->lineTextMPBitmapPath->clear();
    this->mTextMPPicturePreview = QImage();
}

void MainWindow::on_buttonNewConfig_clicked()
{
    AddConfigDialog acd(this);
    int result = acd.exec();
    if (result) {
        if (mFLMSettings->createNewConfig(acd.getConfigName())) {
            mFLMSettings->setActiveConfig(mFLMSettings->size() - 1);
            updateComboConfigs();
            ui->comboConfig->setCurrentIndex(mFLMSettings->size() - 1);
        } else {
            QMessageBox mb;
            mb.setText("Ошибка при сохранении файла. Возможно файл с таким именем уже существует.");
            mb.exec();
        }
    }
}

void MainWindow::on_comboConfig_currentIndexChanged(int index)
{
    mFLMSettings->setActiveConfig(index);
}

void MainWindow::on_checkGridPrintHorLines_toggled(bool checked)
{
    ui->spinGridHorLinesDistance->setEnabled(checked);
}

void MainWindow::on_checkGridPrintVerLines_toggled(bool checked)
{
    ui->spinGridVerLinesDistance->setEnabled(checked);
}
