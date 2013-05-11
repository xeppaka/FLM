#include "addlinedialog.h"
#include "ui_addlinedialog.h"

AddLineDialog::AddLineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLineDialog)
{
    ui->setupUi(this);
}

AddLineDialog::~AddLineDialog()
{
    delete ui;
}

void AddLineDialog::on_buttonBox_accepted()
{
    mResultObject = new ImageObjectLine();
    mResultObject->mFromX = ui->spinStartX->value();
    mResultObject->mToX = ui->spinEndX->value();
    mResultObject->mFromY = ui->spinStartY->value();
    mResultObject->mToY = ui->spinEndY->value();

    done(1);
}

void AddLineDialog::on_buttonBox_rejected()
{
    done(0);
}
