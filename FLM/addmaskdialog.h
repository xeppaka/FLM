#ifndef ADDMASKDIALOG_H
#define ADDMASKDIALOG_H

#include <QDialog>
#include "imagedata.h"

namespace Ui {
    class AddMaskDialog;
}

class AddMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMaskDialog(QWidget *parent = 0);
    ~AddMaskDialog();

public:
    ImageObjectMask* mResultObject;


private:
    Ui::AddMaskDialog *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // ADDMASKDIALOG_H
