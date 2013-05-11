# -------------------------------------------------
# Project created by QtCreator 2009-12-07T22:31:27
# -------------------------------------------------
QT += xml
TARGET = FLM
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    laserthread.cpp \
    falconrender.cpp \
    optionsdialog.cpp \
    imagedata.cpp \
    falconoutputdevice.cpp \
    graphicsoutputdevice.cpp \
    addtextdialog.cpp \
    stdheader.cpp \
    addtextmultiposdialog.cpp \
    addmaskdialog.cpp \
    xmlio.cpp \
    flmsettings.cpp \
    addbitmapdialog.cpp \
    addconfigdialog.cpp \
    addlinedialog.cpp \
    addgriddialog.cpp
HEADERS += mainwindow.h \
    stdheader.h \
    TDLPortIO.h \
    ft2build.h \
    laserthread.h \
    falconrender.h \
    optionsdialog.h \
    imagedata.h \
    outputdevice.h \
    falconoutputdevice.h \
    graphicsoutputdevice.h \
    addtextdialog.h \
    flmsettings.h \
    addtextmultiposdialog.h \
    addmaskdialog.h \
    markdelays.h \
    xmlio.h \
    addbitmapdialog.h \
    addconfigdialog.h \
    addlinedialog.h \
    addgriddialog.h
INCLUDEPATH += ../freetype-2.3.12/include \
    ../LPT
LIBS += -L../LPT/release \
    -L../freetype-2.3.12/objs \
    -lTDLPortIO \
    -lfreetype
FORMS += mainwindow.ui \
    optionsdialog.ui \
    addtextdialog.ui \
    addtextmultiposdialog.ui \
    addmaskdialog.ui \
    addbitmapdialog.ui \
    addconfigdialog.ui \
    addlinedialog.ui \
    addgriddialog.ui
RESOURCES += resources.qrc





















