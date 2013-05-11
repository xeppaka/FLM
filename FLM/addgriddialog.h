#ifndef ADDGRIDDIALOG_H
#define ADDGRIDDIALOG_H

#include "imagedata.h"
#include <QDialog>

namespace Ui {
class AddGridDialog;
}

class AddGridDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddGridDialog(QWidget *parent = 0);
    ~AddGridDialog();

public:
    ImageObjectGrid *mResultObject;

private slots:
    void on_checkPrintHorLines_toggled(bool checked);
    void on_checkPrintVerLines_toggled(bool checked);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::AddGridDialog *ui;
};

#endif // ADDGRIDDIALOG_H
