#ifndef ADDTEXTMULTIPOSDIALOG_H
#define ADDTEXTMULTIPOSDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include "imagedata.h"

namespace Ui {
    class AddTextMultiPosDialog;
}

class AddTextMultiPosDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTextMultiPosDialog(QWidget *parent = 0);
    ~AddTextMultiPosDialog();

private:
    Ui::AddTextMultiPosDialog *ui;

public:
    ImageObjectTextMultiPos* mResultObject;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_comboWayOfDrawing_currentIndexChanged(int index);
    void on_toolButtonFontPath_clicked();
    void on_checkBoxFontInternal_toggled(bool checked);

    void on_buttonPictureLoad_2_clicked();

private:
    QImage image;
    QGraphicsScene gs;
};

#endif // ADDTEXTMULTIPOSDIALOG_H
