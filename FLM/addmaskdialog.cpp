#include "addmaskdialog.h"
#include "ui_addmaskdialog.h"

AddMaskDialog::AddMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMaskDialog)
{
    ui->setupUi(this);
}

AddMaskDialog::~AddMaskDialog()
{
    delete ui;
}

void AddMaskDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectMask();

    mResultObject->mOffsetX = ui->spinOffsetX->value();
    mResultObject->mOffsetY = ui->spinOffsetY->value();
    mResultObject->mWidth = ui->spinWidth->value();
    mResultObject->mHeight = ui->spinHeight->value();
    mResultObject->mDiameter = ui->spinDiameter->value();
    mResultObject->mHorLinesDistance = ui->spinHorLinesDistance->value();
    mResultObject->mVertLinesWidth = ui->spinVertLinesWidth->value();
    mResultObject->mVertLinesDistance = ui->spinVertLinesDistance->value();
    mResultObject->mVertLinesDrawEveryLine = ui->spinVertLinesDrawEveryLine->value();

    done(1);
}

void AddMaskDialog::on_buttonBox_rejected()
{
    done(0);
}
