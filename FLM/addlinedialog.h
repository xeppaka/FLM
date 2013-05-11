#ifndef ADDLINEDIALOG_H
#define ADDLINEDIALOG_H

#include <QDialog>
#include "imagedata.h"

namespace Ui {
    class AddLineDialog;
}

class AddLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLineDialog(QWidget *parent = 0);
    ~AddLineDialog();

public:
    ImageObjectLine *mResultObject;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::AddLineDialog *ui;
};

#endif // ADDLINEDIALOG_H
