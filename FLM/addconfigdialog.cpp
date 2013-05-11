#include "addconfigdialog.h"
#include "ui_addconfigdialog.h"
#include <QMessageBox>

AddConfigDialog::AddConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConfigDialog)
{
    ui->setupUi(this);
}

AddConfigDialog::~AddConfigDialog()
{
    delete ui;
}

void AddConfigDialog::on_buttonBox_accepted()
{
    QString cn = ui->lineConfigName->text();
    if (cn.length() > 0) {
        this->configName = cn;
        done(1);
    } else {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setText("Название конфига не может быть пустым!");
        mb.exec();
    }
}

QString& AddConfigDialog::getConfigName() {
    return this->configName.append(".ini");
}
