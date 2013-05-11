#include "flmsettings.h"
#include <QDir>

FLMSettings::FLMSettings()
{
    allocateMatrix();
    allocateSplines();

    mCorrMatrixLoaded = false;
    activeConfig = 0;
}

FLMSettings::~FLMSettings()
{
    freeSplines();
    freeMatrix();
}

void FLMSettings::load()
{
    // find all .ini files
    QDir dir;
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo f = list.at(i);
        if (f.completeSuffix().size() == 3 && f.completeSuffix().compare("ini") == 0) {
            RenderConfig config;
            config.filename = f.fileName();

            QSettings mSettings(config.filename, QSettings::IniFormat);
            mSettings.beginGroup(QObject::tr("application_state"));
            // Scale slider position
            config.mScale = mSettings.value(KEY_SCALE, DEFAULT_SCALE).toInt();
            if (config.mScale < 1 || config.mScale > 100)
                config.mScale = 1;
            // Render mode state
            config.mRenderMode =
                    static_cast<Render_Mode>(mSettings.
                                             value(KEY_RENDER_MODE,DEFAULT_RENDER_MODE).toInt());
            config.mDrawIdleMotion =
                    mSettings.value(KEY_DRAW_IDLE_MOTION,DEFAULT_DRAW_IDLE_MOTION).toBool();
            config.mCorrTurnedOn = mSettings.value(KEY_CORR_TURNED_ON,DEFAULT_CORR_TURNED_ON).toBool();
            mSettings.endGroup();

            mSettings.beginGroup(QObject::tr("mark_delays"));
            config.mDelays.mDXY_LaserOn =
                    mSettings.value(KEY_DXY_LASER_ON, DEFAULT_DXY_LASER_ON).toInt();
            config.mDelays.mDXY_LaserOff =
                    mSettings.value(KEY_DXY_LASER_OFF, DEFAULT_DXY_LASER_OFF).toInt();
            config.mDelays.mLaserOnDelay =
                    mSettings.value(KEY_LASER_ON_DELAY, DEFAULT_LASER_ON_DELAY).toInt();
            config.mDelays.mLaserOffDelay =
                    mSettings.value(KEY_LASER_OFF_DELAY, DEFAULT_LASER_OFF_DELAY).toInt();
            config.mDelays.mPointDelay =
                    mSettings.value(KEY_POINT_DELAY, DEFAULT_POINT_DELAY).toInt();
            config.mDelays.mQSWPeriod =
                    mSettings.value(KEY_QSW_PERIOD, DEFAULT_QSW_PERIOD).toInt();
            config.mDelays.mQSWDuration =
                    mSettings.value(KEY_QSW_DURATION, DEFAULT_QSW_DURATION).toInt();
            mSettings.endGroup();
            configs.append(config);
        }
    }

    // load default configs if needed
    if (configs.size() <= 0) {
        configs.append(RenderConfig());
        activeConfig = 0;
        save(0);
    }

    //load correction matrix here
    loadCorrectionMatrix();

    QSettings appGlobalSettings(FLMSETTINGS_FILENAME, QSettings::IniFormat);
    appGlobalSettings.beginGroup(QObject::tr("flm"));
    lastUsedConfig = appGlobalSettings.value(KEY_LAST_USED_CONFIG, DEFAULT_LAST_USED_CONFIG).toString();
}

void FLMSettings::save(int num)
{
    if (num < 0 || num >= configs.size())
        return;
    const RenderConfig& config = configs.at(num);
    QSettings mSettings(config.filename, QSettings::IniFormat);
    mSettings.beginGroup(QObject::tr("application_state"));
    mSettings.setValue(KEY_SCALE, config.mScale);
    mSettings.setValue(KEY_RENDER_MODE, config.mRenderMode);
    mSettings.setValue(KEY_DRAW_IDLE_MOTION, config.mDrawIdleMotion);
    mSettings.setValue(KEY_CORR_TURNED_ON, config.mCorrTurnedOn);
    mSettings.endGroup();

    mSettings.beginGroup(QObject::tr("mark_delays"));
    mSettings.setValue(KEY_DXY_LASER_ON, config.mDelays.mDXY_LaserOn);
    mSettings.setValue(KEY_DXY_LASER_OFF, config.mDelays.mDXY_LaserOff);
    mSettings.setValue(KEY_LASER_ON_DELAY, config.mDelays.mLaserOnDelay);
    mSettings.setValue(KEY_LASER_OFF_DELAY, config.mDelays.mLaserOffDelay);
    mSettings.setValue(KEY_POINT_DELAY, config.mDelays.mPointDelay);
    mSettings.setValue(KEY_QSW_PERIOD, config.mDelays.mQSWPeriod);
    mSettings.setValue(KEY_QSW_DURATION, config.mDelays.mQSWDuration);
    mSettings.endGroup();
}

void FLMSettings::saveLastUsedConfig() {
    QSettings appGlobalSettings(FLMSETTINGS_FILENAME, QSettings::IniFormat);
    appGlobalSettings.beginGroup(QObject::tr("flm"));
    appGlobalSettings.setValue(KEY_LAST_USED_CONFIG, getActiveConfig()->filename);
    appGlobalSettings.endGroup();
}

void FLMSettings::loadCorrectionMatrix()
{
    QFile file(CORRECTION_FILE_NAME);
    if (file.open(QIODevice::ReadOnly))
    {
        char* corrBuffer = new char[CORRECTIONS_SIZE];
        if (file.read(corrBuffer, CORRECTIONS_SIZE) != CORRECTIONS_SIZE)
        {
            delete[] corrBuffer;
            file.close();
            mCorrMatrixLoaded = false;
            return;
        }

        file.close();

        convertTo3DArray(reinterpret_cast<int*>(corrBuffer));
        delete[] corrBuffer;

        //allocate K array
        double** K = new double*[SPLINE_X];
        for (int i = 0;i < SPLINE_X;i++)
            K[i] = new double[SPLINE_Y];

        double Fi,Fj,Fm,R;
        int i,j,n;
        const int h = 1024;

        // spline koeff for X
        n = 0;
        do {
            K[1][n] = 0;
            mCx[1][n] = 0;
            i = 2;
            do {
                j = i - 1;
                Fi = mCorrMatrix[i][n][0];
                Fj = mCorrMatrix[j][n][0];
                Fm = mCorrMatrix[j - 1][n][0];
                R = h*4 - h*mCx[j][n];
                mCx[i][n] = h / R;
                K[i][n] = (3*((Fi - Fj) - (Fj - Fm)) / h - K[j][n]) / R;
                ++i;
            } while (i <= 64);
            mCx[64][n] = K[64][n];
            i = 63;
            do {
                mCx[i][n] = K[i][n] - mCx[i][n]*mCx[i + 1][n];
                --i;
            } while (i >= 2);
            ++n;
        } while (n <= 64);

        // spline koeff for Y
        n = 0;
        do {
            K[1][n] = 0;
            mCy[1][n] = 0;
            i = 2;
            do {
                j = i - 1;
                Fi = mCorrMatrix[i][n][1];
                Fj = mCorrMatrix[j][n][1];
                Fm = mCorrMatrix[j - 1][n][1];
                R = h*4 - h*mCy[j][n];
                mCy[i][n] = h / R;
                K[i][n] = (3*((Fi - Fj) - (Fj - Fm)) / h - K[j][n]) / R;
                ++i;
            } while (i <= 64);
            mCy[64][n] = K[64][n];
            i = 63;
            do {
                mCy[i][n] = K[i][n] - mCy[i][n]*mCy[i + 1][n];
                --i;
            } while (i >= 2);
            ++n;
        } while (n <= 64);

        //free K array
        for (int i = 0;i < SPLINE_X;i++)
            delete[] K[i];
        delete[] K;

        mCorrMatrixLoaded = true;
    }
    else
        mCorrMatrixLoaded = false;
}

void FLMSettings::convertTo3DArray(int *buffer)
{
    int MAX_X = SPLINE_X;
    int MAX_Y = SPLINE_Y;
    int MAX_Z = 2;
    for (int x = 0;x < MAX_X;++x)
        for (int y = 0;y < MAX_Y;y++)
            for (int z = 0;z < MAX_Z;z++)
                mCorrMatrix[x][y][z] = buffer[x*MAX_Y*MAX_Z + y*MAX_Z + z];
}

void FLMSettings::allocateMatrix()
{
    int MAX_X = SPLINE_X;
    int MAX_Y = SPLINE_Y;
    int MAX_Z = 2;
    mCorrMatrix = new int**[MAX_X];
    for (int x = 0;x < MAX_X;++x)
    {
        mCorrMatrix[x] = new int*[MAX_Y];
        for (int y = 0;y < MAX_Y;y++)
        {
            mCorrMatrix[x][y] = new int[MAX_Z];
            for (int z = 0;z < MAX_Z;z++)
                mCorrMatrix[x][y][z] = -1;
        }
    }
}

void FLMSettings::freeMatrix()
{
    int MAX_X = SPLINE_X;
    int MAX_Y = SPLINE_Y;

    if (mCorrMatrix)
    {
        for (int x = 0;x < MAX_X;++x)
        {
            for (int y = 0;y < MAX_Y;y++)
                delete[] mCorrMatrix[x][y];
            delete[] mCorrMatrix[x];
        }
        delete[] mCorrMatrix;
        mCorrMatrix = 0;
    }
}

void FLMSettings::allocateSplines()
{
    mCx = new double*[SPLINE_X];
    mCy = new double*[SPLINE_X];
    for (int i = 0;i < SPLINE_X;i++)
    {
        mCx[i] = new double[SPLINE_Y];
        mCy[i] = new double[SPLINE_Y];
    }
}

void FLMSettings::freeSplines()
{
    if (mCx)
    {
        for (int i = 0;i < SPLINE_X;i++)
            delete[] mCx[i];
        delete[] mCx;
        mCx = 0;
    }

    if (mCy)
    {
        for (int i = 0;i < SPLINE_Y;i++)
            delete[] mCy[i];
        delete[] mCy;
        mCy = 0;
    }
}

void FLMSettings::reloadCorrections()
{
    loadCorrectionMatrix();
}

int FLMSettings::size() {
    return configs.size();
}

void FLMSettings::setActiveConfig(int num) {
    if (num < 0 || num >= configs.size())
        return;
    activeConfig = num;
}

RenderConfig* FLMSettings::getActiveConfig() {
    return &configs[activeConfig];
}

RenderConfig* FLMSettings::getConfig(int num) {
    if (num < 0 || num >= configs.size())
        return 0;
    return &configs[num];
}

bool FLMSettings::createNewConfig(QString &name) {
    QFile file(name);
    if (file.exists())
        return false;

    RenderConfig newConfig;
    newConfig.filename = name;
    configs.append(newConfig);
    save(configs.size() - 1);
    return true;
}

int FLMSettings::getLastUsedConfigIndex() {
    for (int i = 0, l = configs.size();i < l;++i)
        if (configs.at(i).filename.compare(lastUsedConfig) == 0)
            return i;
    return 0;
}
