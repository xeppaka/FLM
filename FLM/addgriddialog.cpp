#include "addgriddialog.h"
#include "ui_addgriddialog.h"

AddGridDialog::AddGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddGridDialog)
{
    ui->setupUi(this);
}

AddGridDialog::~AddGridDialog()
{
    delete ui;
}


void AddGridDialog::on_checkPrintHorLines_toggled(bool checked)
{
    ui->spinHorLinesDistance->setEnabled(checked);
}

void AddGridDialog::on_checkPrintVerLines_toggled(bool checked)
{
    ui->spinVerLinesDistance->setEnabled(checked);
}

void AddGridDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectGrid();
    mResultObject->mOffsetX = ui->spinOffsetX->value();
    mResultObject->mOffsetY = ui->spinOffsetY->value();
    mResultObject->mWidth = ui->spinWidth->value();
    mResultObject->mHeight = ui->spinHeight->value();
    mResultObject->mDiameter = ui->spinDiameter->value();
    mResultObject->mPrintHorLines = ui->checkPrintHorLines->isChecked();
    mResultObject->mPrintVertLines = ui->checkPrintVerLines->isChecked();
    mResultObject->mPrintContour = ui->checkPrintContour->isChecked();
    mResultObject->mHorLinesDistance = ui->spinHorLinesDistance->value();
    mResultObject->mVertLinesDistance = ui->spinVerLinesDistance->value();

    done(1);
}

void AddGridDialog::on_buttonBox_rejected()
{
    done(0);
}
