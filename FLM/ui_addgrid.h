/********************************************************************************
** Form generated from reading UI file 'addgrid.ui'
**
** Created: Wed 8. May 00:29:35 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDGRID_H
#define UI_ADDGRID_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_8;
    QSpinBox *spinWidth;
    QSpinBox *spinHeight;
    QSpinBox *spinOffsetY;
    QSpinBox *spinOffsetX;
    QLabel *label;
    QSpinBox *spinDiameter;
    QLabel *label_3;
    QLabel *label_7;
    QLabel *label_2;
    QGroupBox *groupBox;
    QCheckBox *checkPrintHorLines;
    QSpinBox *spinHorLinesDistance;
    QLabel *label_4;
    QGroupBox *groupBox_2;
    QSpinBox *spinVerLinesDistance;
    QLabel *label_10;
    QCheckBox *checkPrintVerLines;
    QCheckBox *checkBox_3;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(361, 381);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(11, 340, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_8 = new QLabel(Dialog);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(190, 25, 81, 16));
        spinWidth = new QSpinBox(Dialog);
        spinWidth->setObjectName(QString::fromUtf8("spinWidth"));
        spinWidth->setGeometry(QRect(100, 44, 71, 22));
        spinWidth->setMinimum(1);
        spinWidth->setMaximum(99999);
        spinHeight = new QSpinBox(Dialog);
        spinHeight->setObjectName(QString::fromUtf8("spinHeight"));
        spinHeight->setGeometry(QRect(280, 44, 71, 22));
        spinHeight->setMinimum(1);
        spinHeight->setMaximum(99999);
        spinOffsetY = new QSpinBox(Dialog);
        spinOffsetY->setObjectName(QString::fromUtf8("spinOffsetY"));
        spinOffsetY->setGeometry(QRect(280, 20, 71, 22));
        spinOffsetY->setMaximum(99999);
        spinOffsetX = new QSpinBox(Dialog);
        spinOffsetX->setObjectName(QString::fromUtf8("spinOffsetX"));
        spinOffsetX->setGeometry(QRect(100, 20, 71, 22));
        spinOffsetX->setMaximum(99999);
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 47, 53, 16));
        spinDiameter = new QSpinBox(Dialog);
        spinDiameter->setObjectName(QString::fromUtf8("spinDiameter"));
        spinDiameter->setGeometry(QRect(100, 68, 71, 22));
        spinDiameter->setMaximum(99999);
        spinDiameter->setValue(0);
        label_3 = new QLabel(Dialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 72, 61, 16));
        label_7 = new QLabel(Dialog);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 25, 81, 16));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(190, 47, 51, 20));
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 131, 341, 81));
        checkPrintHorLines = new QCheckBox(groupBox);
        checkPrintHorLines->setObjectName(QString::fromUtf8("checkPrintHorLines"));
        checkPrintHorLines->setGeometry(QRect(16, 19, 301, 20));
        spinHorLinesDistance = new QSpinBox(groupBox);
        spinHorLinesDistance->setObjectName(QString::fromUtf8("spinHorLinesDistance"));
        spinHorLinesDistance->setGeometry(QRect(250, 43, 71, 22));
        spinHorLinesDistance->setMaximum(99999);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(17, 40, 211, 31));
        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 220, 341, 81));
        spinVerLinesDistance = new QSpinBox(groupBox_2);
        spinVerLinesDistance->setObjectName(QString::fromUtf8("spinVerLinesDistance"));
        spinVerLinesDistance->setGeometry(QRect(250, 44, 71, 22));
        spinVerLinesDistance->setMaximum(99999);
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(17, 40, 211, 31));
        checkPrintVerLines = new QCheckBox(groupBox_2);
        checkPrintVerLines->setObjectName(QString::fromUtf8("checkPrintVerLines"));
        checkPrintVerLines->setGeometry(QRect(15, 20, 301, 20));
        checkBox_3 = new QCheckBox(Dialog);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setGeometry(QRect(10, 100, 121, 17));

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Dialog", "\320\241\320\274\320\265\321\211\320\265\320\275\320\270\320\265 Y:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "\320\250\320\270\321\200\320\270\320\275\320\260:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Dialog", "\320\224\320\270\320\260\320\274\320\265\321\202\321\200:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Dialog", "\320\241\320\274\320\265\321\211\320\265\320\275\320\270\320\265 X:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "\320\222\321\213\321\201\320\276\321\202\320\260:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Dialog", "\320\241\320\262\320\276\320\271\321\201\321\202\320\262\320\260 \320\263\320\276\321\200\320\270\320\267\320\276\320\275\321\202\320\260\320\273\321\214\320\275\321\213\321\205 \320\273\320\270\320\275\320\270\320\271", 0, QApplication::UnicodeUTF8));
        checkPrintHorLines->setText(QApplication::translate("Dialog", "\320\240\320\270\321\201\320\276\320\262\320\260\321\202\321\214 \320\273\320\270\320\275\320\270\320\270", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Dialog", "<html><head/><body><p>\320\240\320\260\321\201\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\274\320\265\320\266\320\264\321\203 \320\273\320\270\320\275\320\270\321\217\320\274\320\270</p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Dialog", "\320\241\320\262\320\276\320\271\321\201\321\202\320\262\320\260 \320\262\320\265\321\200\321\202\320\270\320\272\320\260\320\273\321\214\320\275\321\213\321\205 \320\273\320\270\320\275\320\270\320\271", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Dialog", "\320\240\320\260\321\201\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\274\320\265\320\266\320\264\321\203 \320\273\320\270\320\275\320\270\321\217\320\274\320\270", 0, QApplication::UnicodeUTF8));
        checkPrintVerLines->setText(QApplication::translate("Dialog", "\320\240\320\270\321\201\320\276\320\262\320\260\321\202\321\214 \320\273\320\270\320\275\320\270\320\270", 0, QApplication::UnicodeUTF8));
        checkBox_3->setText(QApplication::translate("Dialog", "\320\240\320\270\321\201\320\276\320\262\320\260\321\202\321\214 \320\272\320\276\320\275\321\202\321\203\321\200", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDGRID_H
