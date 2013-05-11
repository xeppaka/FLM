#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stdheader.h"
#include "flmsettings.h"
#include "falconoutputdevice.h"
#include "graphicsoutputdevice.h"
#include "falconrender.h"
#include "imagedata.h"
#include "laserthread.h"
#include "xmlio.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsView>
#include <QSettings>
#include <QListWidget>
#include <QTimer>
#include <QTime>
#include <QCloseEvent>

#define GRAPHICS_SCALE 0.007
#define SCALE_FORMULA_CONST 0.04651687056553627644480790817544
#define ORGANIZATION "SOLAR-GROUP"
#define APPLICATION "FALCON LASER MANAGER"

#define TIMER_REFRESH 50
#define WINDOW_NAME "Falcon laser manager"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;

private slots:
    void on_statusCorrectionsReloadButton_clicked();
    void on_buttonSaveAs_clicked();
    void on_buttonSave_clicked();
    void on_buttonLoad_clicked();
    void on_buttonMakeCenter_clicked();
    void on_buttonAddMask_clicked();
    void on_toolButtonFontPath_2_clicked();
    void on_checkBoxFontInternal_2_toggled(bool checked);
    void on_comboWayOfDrawing_2_currentIndexChanged(int index);
    void on_buttonShowTabWidget_clicked();
    void on_buttonAddTextMultiPos_clicked();
    void on_buttonStop_clicked();
    void on_comboWayOfDrawing_currentIndexChanged(int index);
    void on_sliderScale_valueChanged(int value);
    void on_checkBoxFontInternal_toggled(bool checked);
    void on_toolButtonFontPath_clicked();
    void on_buttonAddText_clicked();
    void on_buttonRemoveImageObject_clicked();
    void on_buttonSaveImageObject_clicked();
    void on_listObjects_itemClicked(QListWidgetItem* item);
    void on_buttonOptions_clicked();
    void on_buttonReset_clicked();
    void on_buttonStart_clicked();

//----------MY AREA----------------
public:

private:
    FalconOutputDevice *mFOD;
    GraphicsOutputDevice *mGOD;
    FalconRender *mFalconRender;
    FLMSettings *mFLMSettings;
    LaserThread *mLaserThread;

    ImageData *mImageData;

    QTimer *mTimer;
    QTime *mTime;

    QMatrix *mScreenMatrix;
    QMatrix *mFieldMatrix;

    Xmlio *mXmlio;
    QString mCurrFileName;
    bool mSaved;

    // picture preview for one picture
    QImage mOnePicturePreview;
    QGraphicsScene mOnePreviewScene;
    // picture preview for text multi pos
    QImage mTextMPPicturePreview;
    QGraphicsScene mTextMPPreviewScene;

    void updateCurrFileName();
    void updateComboConfigs();
    void updateControls(bool deviceReady, DeviceInfo *devInfo);
    void setDeviceInformation(DeviceInfo *devInfo);
    void clearDeviceInformation();
    bool renderImageOnScreen();
    bool renderImageOnScreen(Render_Mode mode);
    bool renderImageOnLaser();
    void settingsToUi();
    void uiToSettings();
    void updateCorrectionsStatus();
    void switchUiForLaserWork();
    void switchUiForIdle();
    void updateListWidget();
    void showErrorRenderingMessage();
    bool showImageOutWarning();
    bool showSaveQuestionMessage();
    void showSaveErrorMessage();
    void showOpenErrorMessage();

private slots:
    void on_LaserThread_finished();
    void on_Timer_timeout();
    void on_buttonAddBitmap_clicked();
    void on_radioPictureScale_toggled(bool checked);
    void on_radioPictureFixed_toggled(bool checked);
    void on_buttonPictureLoad_clicked();
    void on_buttonPictureClear_clicked();
    void on_lineBitmapPath_returnPressed();
    void on_buttonTextMPPictureLoad_clicked();
    void on_lineTextMPBitmapPath_returnPressed();
    void on_radioTextMPPictureScale_toggled(bool checked);
    void on_radioTextMPPictureFixed_toggled(bool checked);
    void on_buttonTextMPPictureClear_clicked();
    void on_buttonNewConfig_clicked();
    void on_comboConfig_currentIndexChanged(int index);
    void on_buttonAddLine_clicked();
    void on_buttonAddGrid_clicked();
    void on_checkGridPrintHorLines_toggled(bool checked);
    void on_checkGridPrintVerLines_toggled(bool checked);
};

#endif // MAINWINDOW_H
