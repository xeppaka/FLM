#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include "falconrender.h"
#include "flmsettings.h"

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog {
    Q_OBJECT
public:
    OptionsDialog(FLMSettings *settings, QWidget *parent = 0);
    ~OptionsDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::OptionsDialog *ui;
    FLMSettings *mSettings;

private slots:
    void on_radioLaserEmulation_toggled(bool checked);
    void on_radioFastDraw_toggled(bool checked);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // OPTIONSDIALOG_H
