#include "addbitmapdialog.h"
#include "ui_addbitmapdialog.h"

#include <QFileDialog>
#include <QPixmap>

AddBitmapDialog::AddBitmapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBitmapDialog)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&gs);
}

AddBitmapDialog::~AddBitmapDialog()
{
    delete ui;
}

void AddBitmapDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectPicture();

    mResultObject->mImage = image;
    if (ui->radioScale->isChecked()) {
        mResultObject->mActiveSize = SIZE_SCALED;
        mResultObject->mScaleX = ui->doubleScaleX->value();
        mResultObject->mScaleY = ui->doubleScaleY->value();
    }
    else {
        mResultObject->mActiveSize = SIZE_FIXED;
        mResultObject->mWidth = ui->spinWidth->value();
        mResultObject->mHeight = ui->spinHeight->value();
    }

    mResultObject->mShiftX = ui->spinOffsetX->value();
    mResultObject->mShiftY = ui->spinOffsetY->value();
    mResultObject->mDrawEveryLine = ui->spinDrawEveryLine->value();
    done(1);
}

void AddBitmapDialog::on_buttonBox_rejected()
{
    done(0);
}

void AddBitmapDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open bitmap"),
                                                    tr(""),
                                                     tr("Image file (*.bmp *.jpg *.jpeg *.png)"));
    if (fileName.length())
        ui->lineBitmapPath->setText(fileName);
    if (image.load(fileName)) {
        gs.clear();
        QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->graphicsView->width(),
                                                          ui->graphicsView->height(),
                                                          Qt::KeepAspectRatio);
        gs.addPixmap(pixmap);
    }
}

void AddBitmapDialog::on_radioScale_toggled(bool checked)
{
    if (checked) {
        ui->doubleScaleX->setEnabled(true);
        ui->doubleScaleY->setEnabled(true);
    } else {
        ui->doubleScaleX->setEnabled(false);
        ui->doubleScaleY->setEnabled(false);
    }
}

void AddBitmapDialog::on_radioFixed_toggled(bool checked)
{
    if (checked) {
        ui->spinWidth->setEnabled(true);
        ui->spinHeight->setEnabled(true);
    } else {
        ui->spinWidth->setEnabled(false);
        ui->spinHeight->setEnabled(false);
    }
}
