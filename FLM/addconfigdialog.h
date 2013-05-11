#ifndef ADDCONFIGDIALOG_H
#define ADDCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class AddConfigDialog;
}

class AddConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddConfigDialog(QWidget *parent = 0);
    ~AddConfigDialog();

    QString& getConfigName();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddConfigDialog *ui;
    QString configName;
};

#endif // ADDCONFIGDIALOG_H
