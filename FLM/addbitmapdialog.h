#ifndef addbitmapdialog_H
#define addbitmapdialog_H

#include <QDialog>
#include <QGraphicsScene>
#include <QImage>
#include <imagedata.h>

namespace Ui {
    class AddBitmapDialog;
}

class AddBitmapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddBitmapDialog(QWidget *parent = 0);
    ~AddBitmapDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_clicked();

    void on_radioScale_toggled(bool checked);

    void on_radioFixed_toggled(bool checked);

private:
    Ui::AddBitmapDialog *ui;
    QGraphicsScene gs;
    QImage image;

public:
    ImageObjectPicture *mResultObject;
};

#endif // addbitmapdialog_H
