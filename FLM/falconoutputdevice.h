#ifndef FALCONLASERMANAGEMENT_H
#define FALCONLASERMANAGEMENT_H

#include <windows.h>
#include "TDLPortIO.h"
#include "outputdevice.h"
#include <QString>
#include <QVector>
#include <QPoint>

#define F_CLOCK 14.7456
#define BUFFER_INITIAL_SIZE 16777216

enum BUFFER_ADDRESS {
    BUFFER_ADDRESS0 = 0,
    BUFFER_ADDRESS2 = 2,
    BUFFER_ADDRESS3 = 3
};

enum LAST_ERROR {
    WRITE_ERROR_DEVICE_BUFFER_FULL,
    WRITE_ERROR_DEVICE_NOT_READY,
    WRITE_ERROR_CRC
};

struct DeviceInfo
{
    //Falcon control node characteristics
    QString mVersion;
    int mCPUVersion;
    int mFPGAVersion;
    int mInBufferSize;
    int mOutBufferSize;
    char mDAWidth;
};

class FalconOutputDevice : public OutputDevice
{
public:
    FalconOutputDevice();
    virtual ~FalconOutputDevice();

    //constants
    static const int BUFFER_WRITE_MAX = 2040;
    static const int BUFFER_WRITE_RETRY_COUNT = 10000;

    DeviceInfo mDeviceInfo;

    LAST_ERROR mLastError;
    int mPercentDone;

private:
//    uchar* mCommandBuffer;
//    int mBufferPosition;

public:
    QVector<uchar>* mCommandBuffer;
    int mBufferPosition;
    void openDevice();
    void closeDevice();
    bool isDeviceReady();
    void reset();
    int getCounter();
    virtual bool begin(MarkDelays *delays);
    virtual bool end();
    virtual bool flush();
    virtual void interrupt();

    bool writeCommandToBuffer(uchar b1, uchar b2, uchar b3, uchar b4);
    virtual bool writeXYAndLaserOff(int x, int y);
    virtual bool writeXYAndLaserOn(int x, int y);
    virtual bool writeXAndLaserOffDelay(int x);
    virtual bool writeXAndLaserOnDelay(int x);
    virtual bool writeXAndLaserOff(int x);
    virtual bool writeXAndLaserOn(int x);
    virtual bool writeYAndLaserOffDelay(int y);
    virtual bool writeYAndLaserOnDelay(int y);
    virtual bool writeYAndLaserOff(int y);
    virtual bool writeYAndLaserOn(int y);

    /* Permanent delay. command: 20 00 TTTT      */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writePermDelay(int delay);

    /* One time delay. command: 20 04 TTTT       */
    /*                                           */
    /* input: delay - delay time in microseconds */
    /*                                           */
    virtual bool writeOnceDelay(int delay);

    virtual bool writeLaserOnDelay(int delay);
    virtual bool writeLaserOffDelay(int delay);

    virtual bool writeQSWPeriod(int delay);
    virtual bool writeQSWDuration(int delay);

    // this functions are not supported by this class
    virtual bool moveTo(int x, int y);

    virtual bool lineTo(int x, int y);

    // Beizer arcs
    virtual bool conicTo(int controlX, int controlY,
                         int toX, int toY);

    virtual bool cubicTo(int control1X, int control1Y,
                         int control2X, int control2Y,
                         int toX, int toY);
    virtual bool segmentTo(int centerX, int centerY, int toX, int toY);

private:
    int mPort;
    int mCtrlRegister;
    bool mDeviceReady;
    bool mInterrupted;
    QPoint mCurMarkVector;
    bool mLaserOn;
    int mCounter;

    void readDeviceInformation();
    uchar calcCRC8(uchar val);
    bool writeDeviceCommand(uchar b1, uchar b2, uchar b3, uchar b4);
    void writeDeviceAddress(BUFFER_ADDRESS bufAddress);
    void writeDeviceByte(uchar val);
    void readDeviceBuffer(uchar* buffer, int count);
    bool readDeviceErrorCRC();
    bool readDeviceErrorDNR();
    uchar readDeviceByte();
};

#endif // FALCONLASERMANAGEMENT_H
