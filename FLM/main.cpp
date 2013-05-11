#include "stdheader.h"
#include "falconoutputdevice.h"
#include "laserthread.h"

#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include <QPlastiqueStyle>

int main(int argc, char *argv[])
{
    QTextCodec *cyrillicCodec = QTextCodec::codecForName("CP1251");
    QTextCodec::setCodecForTr(cyrillicCodec);
    QTextCodec::setCodecForLocale(cyrillicCodec);
    QTextCodec::setCodecForCStrings(cyrillicCodec);
    QApplication a(argc, argv);
    //a.setStyle(new QPlastiqueStyle());
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
