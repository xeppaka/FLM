#ifndef ADDTEXTDIALOG_H
#define ADDTEXTDIALOG_H

#include <QDialog>
#include "imagedata.h"

namespace Ui {
    class AddTextDialog;
}

class AddTextDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddTextDialog(QWidget *parent = 0);
    ~AddTextDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddTextDialog *ui;

public:
    ImageObjectText* mResultObject;

private slots:
    void on_comboWayOfDrawing_currentIndexChanged(int index);
    void on_checkBoxFontInternal_toggled(bool checked);
    void on_toolButtonFont_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // ADDTEXTDIALOG_H
