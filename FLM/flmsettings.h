#ifndef FLMSETTINGS_H
#define FLMSETTINGS_H

#include <QSettings>
#include <QFile>

#include "graphicsoutputdevice.h"
#include "markdelays.h"

#define SPLINE_X 65
#define SPLINE_Y 65
#define CORRECTIONS_SIZE SPLINE_X*SPLINE_Y*8

#define KEY_SCALE "SCALE_PERCENT"
#define KEY_RENDER_MODE "RENDER_MODE"
#define KEY_DRAW_IDLE_MOTION "DRAW_IDLE_MOTION"
#define KEY_CORR_TURNED_ON "CORR_TURNED_ON"
#define KEY_LASER_ON_DELAY "LASER_ON_DELAY"
#define KEY_LASER_OFF_DELAY "LASER_OFF_DELAY"
#define KEY_DXY_LASER_ON "DXY_LASER_ON_DELAY"
#define KEY_DXY_LASER_OFF "DXY_LASER_OFF_DELAY"
#define KEY_POINT_DELAY "POINT_DELAY"
#define KEY_QSW_PERIOD "QSW_PERIOD"
#define KEY_QSW_DURATION "QSW_DURATION"
#define KEY_LAST_USED_CONFIG "LAST_USED_CONFIG"

#define DEFAULT_SCALE 1
#define DEFAULT_RENDER_MODE FAST
#define DEFAULT_DRAW_IDLE_MOTION false
#define DEFAULT_CORR_TURNED_ON true
#define DEFAULT_LASER_ON_DELAY 10
#define DEFAULT_LASER_OFF_DELAY 650
#define DEFAULT_DXY_LASER_ON 10
#define DEFAULT_DXY_LASER_OFF 600
#define DEFAULT_POINT_DELAY 200
#define DEFAULT_QSW_PERIOD 300
#define DEFAULT_QSW_DURATION 10
#define DEFAULT_LAST_USED_CONFIG "default.ini"

#define CORRECTION_FILE_NAME "corr.asc"
#define DEFAULT_FILENAME "default.ini"
#define FLMSETTINGS_FILENAME ".config"

struct RenderConfig {
    RenderConfig() {
        mScale = DEFAULT_SCALE;
        mRenderMode = DEFAULT_RENDER_MODE;
        mDrawIdleMotion = DEFAULT_DRAW_IDLE_MOTION;
        mDelays.mDXY_LaserOff = DEFAULT_DXY_LASER_OFF;
        mDelays.mDXY_LaserOn = DEFAULT_DXY_LASER_ON;
        mDelays.mLaserOffDelay = DEFAULT_LASER_OFF_DELAY;
        mDelays.mLaserOnDelay = DEFAULT_LASER_ON_DELAY;
        mDelays.mPointDelay = DEFAULT_POINT_DELAY;
        mDelays.mQSWDuration = DEFAULT_QSW_DURATION;
        mDelays.mQSWPeriod = DEFAULT_QSW_PERIOD;
        filename = DEFAULT_FILENAME;
    }

    MarkDelays mDelays;
    int mScale;
    Render_Mode mRenderMode;
    bool mDrawIdleMotion;
    bool mCorrTurnedOn;
    QString filename;
};

class FLMSettings
{
public:
    FLMSettings();
    ~FLMSettings();
    void load();
    void reloadCorrections();
    void save(int num);
    void saveLastUsedConfig();
    void allocateSplines();
    void freeSplines();
    void allocateMatrix();
    void freeMatrix();

    int*** mCorrMatrix;
    double** mCx;
    double** mCy;
    bool mCorrMatrixLoaded;

    void setActiveConfig(int num);
    RenderConfig *getActiveConfig();
    RenderConfig *getConfig(int num);
    int getLastUsedConfigIndex();
    bool createNewConfig(QString &name);
    int size();

private:
    void loadCorrectionMatrix();
    void convertTo3DArray(int *buffer);

    QList<RenderConfig> configs;
    int activeConfig;
    QString lastUsedConfig;
};

#endif // FLMSETTINGS_H
