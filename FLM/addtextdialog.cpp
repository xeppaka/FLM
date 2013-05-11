#include "addtextdialog.h"
#include "ui_addtextdialog.h"
#include "falconrender.h"
#include <QFileDialog>

AddTextDialog::AddTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTextDialog)
{
    ui->setupUi(this);
}

AddTextDialog::~AddTextDialog()
{
    delete ui;
}

void AddTextDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AddTextDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectText();

    mResultObject->mText = ui->lineText->text();
    if (ui->checkBoxFontInternal->isChecked())
        mResultObject->mFont = INTERNAL_FONT;
    else
    {
        if (ui->lineFont->text().length() != 0)
            mResultObject->mFont = ui->lineFont->text();
        else
            mResultObject->mFont = INTERNAL_FONT;
    }
    mResultObject->mSize = ui->doubleSpinSize->value();
    mResultObject->mOffsetX = ui->spinTextX->value();
    mResultObject->mOffsetY = ui->spinTextY->value();
    mResultObject->mRadius = ui->spinRadius->value();
    mResultObject->mAdditionalLetterSpace = ui->spinAdditinalLetterSpace->value();

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
    done(1);
}

void AddTextDialog::on_buttonBox_rejected()
{
    done(0);
}

void AddTextDialog::on_toolButtonFont_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open font"),
                                                    tr(""),
                                                     tr("Truetype (*.ttf);;All files (*.*)"));
    if (fileName.length())
        ui->lineFont->setText(fileName);
}

void AddTextDialog::on_checkBoxFontInternal_toggled(bool checked)
{
    if (checked)
    {
        ui->lineFont->setEnabled(false);
        ui->toolButtonFont->setEnabled(false);
    }
    else
    {
        ui->lineFont->setEnabled(true);
        ui->toolButtonFont->setEnabled(true);
    }
}

void AddTextDialog::on_comboWayOfDrawing_currentIndexChanged(int index)
{
    if (index == 0 || index == 2)
        ui->spinPrintEveryLine->setEnabled(true);
    else
        ui->spinPrintEveryLine->setEnabled(false);
}
