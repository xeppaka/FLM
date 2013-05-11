#include "stdheader.h"
#include "FalconOutputDevice.h"

FalconOutputDevice::FalconOutputDevice()
{
    mCommandBuffer = new QVector<uchar>(0);

    mPort = 0x378;
    mCtrlRegister = 0x778;
    mDeviceInfo.mVersion[0] = 0;
    mDeviceInfo.mCPUVersion = 0;
    mDeviceInfo.mFPGAVersion = 0;
    mDeviceInfo.mInBufferSize = 0;
    mDeviceInfo.mOutBufferSize = 0;
    mDeviceInfo.mDAWidth = 0;
    mDeviceReady = false;
    mCounter = 0;
}

FalconOutputDevice::~FalconOutputDevice()
{
    delete mCommandBuffer;
    closeDevice();
}

bool FalconOutputDevice::begin(MarkDelays *delays)
{
    mCommandBuffer->clear();
    mCurMarkVector.rx() = mCurMarkVector.ry() = 0;
    mLaserOn = false;

    //  mOutDevice->writeCommandToBuffer(0x20,0x02,0x00,0x01);
    if (!writeLaserOnDelay(delays->mLaserOnDelay))
        return false;
    //  mOutDevice->writeCommandToBuffer(0x20,0x03,0x00,0x41);
    if (!writeLaserOffDelay(delays->mLaserOffDelay))
        return false;
    //  mOutDevice->writeCommandToBuffer(0x03,0x04,0x08,0x10);
    if (!writeQSWPeriod(delays->mQSWPeriod))
        return false;
    //  mOutDevice->writeCommandToBuffer(0x03,0x05,0x04,0x4B);
    if (!writeQSWDuration(delays->mQSWDuration))
        return false;
    //  mOutDevice->writeCommandToBuffer(0x20,0x00,0xFE,0xCC);
    if (!writePermDelay(delays->mPointDelay))
        return false;

    return true;
}

bool FalconOutputDevice::end()
{
    return true;
}

bool FalconOutputDevice::flush()
{
    if (!mDeviceReady)
        return false;

    int commands = mCommandBuffer->size();

    if (commands % 4 != 0)
        return false;

    commands /= 4;

    mPercentDone = 0;
    mInterrupted = false;

    for (int i = 0;i < commands;i++)
    {
        if (mInterrupted)
            break;

        if (!writeDeviceCommand(mCommandBuffer->at(i*4),mCommandBuffer->at(i*4 + 1),
                                mCommandBuffer->at(i*4 + 2),mCommandBuffer->at(i*4 + 3)))
            return false;
        mPercentDone = (static_cast<float>(i) / commands) * 100;
    }

    // if interrupted return to start position
    if (mInterrupted)
    {
        if (!writeXAndLaserOff(START_POS_X))
            return false;
        if (!writeYAndLaserOff(START_POS_Y))
            return false;
        int pos = mCommandBuffer->size() - 7;
        if (!writeDeviceCommand(mCommandBuffer->at(pos),mCommandBuffer->at(pos + 1),
                                mCommandBuffer->at(pos + 2),mCommandBuffer->at(pos + 3)))
            return false;
        if (!writeDeviceCommand(mCommandBuffer->at(pos + 4),mCommandBuffer->at(pos + 5),
                                mCommandBuffer->at(pos + 6),mCommandBuffer->at(pos + 7)))
            return false;
    }

    ++mCounter;
    return true;
}

void FalconOutputDevice::interrupt()
{
    mInterrupted = true;
}

int FalconOutputDevice::getCounter()
{
    return mCounter;
}

bool FalconOutputDevice::writeCommandToBuffer(uchar b1, uchar b2, uchar b3, uchar b4)
{
    mCommandBuffer->append(b1);
    mCommandBuffer->append(b2);
    mCommandBuffer->append(b3);
    mCommandBuffer->append(b4);
    return true;
}

bool FalconOutputDevice::writeXYAndLaserOff(int x, int y)
{
    uchar b1, b2, b3, b4;
    x = x >> 4; y = y >> 4;
    b1 = (0x04 << 4) | ((x >> 8) & 0x0F);
    b2 = x & 0xFF;
    b3 = (y >> 8) & 0x0F;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeXYAndLaserOn(int x, int y)
{
    uchar b1, b2, b3, b4;
    b1 = (0x0C << 4) | ((x >> 8) & 0x0F);
    b2 = x & 0xFF;
    b3 = (y >> 8) & 0x0F;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeXAndLaserOffDelay(int x)
{
    uchar b1, b2, b3, b4;
    b1 = 0x60;
    b2 = 0x00;
    b3 = (x >> 8) & 0xFF;
    b4 = x & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeXAndLaserOnDelay(int x)
{
    uchar b1, b2, b3, b4;
    b1 = 0xE0;
    b2 = 0x00;
    b3 = (x >> 8) & 0xFF;
    b4 = x & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeXAndLaserOff(int x)
{
    uchar b1, b2, b3, b4;
    b1 = 0x60;
    b2 = 0x02;
    b3 = (x >> 8) & 0xFF;
    b4 = x & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeXAndLaserOn(int x)
{
    uchar b1, b2, b3, b4;
    b1 = 0xE0;
    b2 = 0x02;
    b3 = (x >> 8) & 0xFF;
    b4 = x & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeYAndLaserOffDelay(int y)
{
    uchar b1, b2, b3, b4;
    b1 = 0x60;
    b2 = 0x01;
    b3 = (y >> 8) & 0xFF;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeYAndLaserOnDelay(int y)
{
    uchar b1, b2, b3, b4;
    b1 = 0xE0;
    b2 = 0x01;
    b3 = (y >> 8) & 0xFF;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeYAndLaserOff(int y)
{
    uchar b1, b2, b3, b4;
    b1 = 0x60;
    b2 = 0x03;
    b3 = (y >> 8) & 0xFF;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeYAndLaserOn(int y)
{
    uchar b1, b2, b3, b4;
    b1 = 0xE0;
    b2 = 0x03;
    b3 = (y >> 8) & 0xFF;
    b4 = y & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writePermDelay(int delay)
{
    int cdelay = 65536 - F_CLOCK * delay / 12;
    uchar b1, b2, b3, b4;
    b1 = 0x20;
    b2 = 0x00;
    b3 = (cdelay >> 8) & 0xFF;
    b4 = cdelay & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeOnceDelay(int delay)
{
    int cdelay = 65536 - F_CLOCK * delay / 12;
    uchar b1, b2, b3, b4;
    b1 = 0x20;
    b2 = 0x04;
    b3 = (cdelay >> 8) & 0xFF;
    b4 = cdelay & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeLaserOnDelay(int delay)
{
    uchar b1, b2, b3, b4;
    b1 = 0x20;
    b2 = 0x02;
    b3 = 0x00;
    b4 = static_cast<uchar>(round(delay/10.0));
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeLaserOffDelay(int delay)
{
    uchar b1, b2, b3, b4;
    b1 = 0x20;
    b2 = 0x03;
    b3 = 0x00;
    b4 = static_cast<uchar>(round(delay/10.0));
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeQSWPeriod(int delay)
{
    int cdelay = F_CLOCK * delay - 147;
    uchar b1, b2, b3, b4;
    b1 = 0x03;
    b2 = 0x04;
    b3 = (cdelay >> 8) & 0xFF;
    b4 = cdelay & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

bool FalconOutputDevice::writeQSWDuration(int delay)
{
    int cdelay = F_CLOCK * delay - 6;
    uchar b1, b2, b3, b4;
    b1 = 0x03;
    b2 = 0x05;
    b3 = (cdelay >> 8) & 0xFF;
    b4 = cdelay & 0xFF;
    return writeCommandToBuffer(b1, b2, b3, b4);
}

void FalconOutputDevice::openDevice()
{
    if (mDeviceReady)
        return;

    if (!IsDriverOpened())
        OpenLPTDriver();

    if (IsDriverOpened())
    {
        readDeviceInformation();
        if (QString(mDeviceInfo.mVersion).contains("falcon",Qt::CaseInsensitive))
            mDeviceReady = true;
    }
}

void FalconOutputDevice::closeDevice()
{
    mDeviceReady = false;

    if (IsDriverOpened())
        CloseLPTDriver();
}

bool FalconOutputDevice::isDeviceReady()
{
    return mDeviceReady;
}

void FalconOutputDevice::reset()
{
    if (!IsDriverOpened())
        return;

    mDeviceReady = false;

    WritePort(mCtrlRegister + 0x02,0x00);//переключение на SPP
    Sleep(5);
    WritePort(mCtrlRegister + 0x02,0x80);//переключение на EPP

    WritePort(mPort+0x02,0x00);//сброс
    WritePort(mPort+0x02,0x04);//сброс
    WritePort(mPort,0xFF);//0xFF на выход порта

    openDevice();
}

void FalconOutputDevice::writeDeviceAddress(BUFFER_ADDRESS bufAddress)
{
    WritePort(mPort + 0x03,bufAddress);
}

void FalconOutputDevice::writeDeviceByte(uchar val)
{
    WritePort(mPort + 0x04,val);
}

bool FalconOutputDevice::readDeviceErrorCRC()
{
    return ((ReadPort(mPort + 0x01) & 0x20) == 0);
}

bool FalconOutputDevice::readDeviceErrorDNR()
{
    return ((ReadPort(mPort + 0x01) & 0x08) > 0);
}

uchar FalconOutputDevice::readDeviceByte()
{
    return ReadPort(mPort + 0x04);
}

void FalconOutputDevice::readDeviceBuffer(uchar* buffer, int count)
{
    for(int i = 0;i < count;i++)
        buffer[i] = ReadPort(mPort + 0x04);
}

void FalconOutputDevice::readDeviceInformation()
{
    writeDeviceAddress(BUFFER_ADDRESS2);
    uchar info[53];
    mDeviceInfo.mVersion.clear();
    readDeviceBuffer(info,53);
    for (int i = 0;i < 31;i++)
        mDeviceInfo.mVersion.push_back(info[i + 0x01]);
    mDeviceInfo.mCPUVersion = ((int*)info)[8];
    mDeviceInfo.mFPGAVersion = ((int*)info)[9];
    mDeviceInfo.mInBufferSize = ((int*)info)[10];
    mDeviceInfo.mOutBufferSize = ((int*)info)[11];
    mDeviceInfo.mDAWidth = info[52];
    writeDeviceAddress(BUFFER_ADDRESS0);
}

bool FalconOutputDevice::writeDeviceCommand(uchar b1, uchar b2, uchar b3, uchar b4)
{
    uchar crc;
    crc = calcCRC8(b1);
    crc = calcCRC8(crc ^ b2);
    crc = calcCRC8(crc ^ b3);
    crc = calcCRC8(crc ^ b4);

    //check if device is ready for input
    bool ready = false;
    int writeBufferSize = 0;
    for (int retry = 0;retry < BUFFER_WRITE_RETRY_COUNT && !ready;retry++)
    {
        writeDeviceAddress(BUFFER_ADDRESS0);
        writeBufferSize = readDeviceByte();
        writeBufferSize = (writeBufferSize & 0x07) << 8;
        writeBufferSize |= readDeviceByte();
        ready = (writeBufferSize <= (mDeviceInfo.mInBufferSize - 8));
    }

    if (!ready)
    {
        mLastError = WRITE_ERROR_DEVICE_BUFFER_FULL;
        qDebug() << "Device buffer is full. Error in: FalconLaserManager::writeDeviceCommand("
                 << b1 << "," << b2 << "," << b3 << "," << b4 << ")";
        return false;
    }

    bool err = true;
    for (int retry = 0;retry < BUFFER_WRITE_RETRY_COUNT && err;retry++)
    {
        writeDeviceAddress(BUFFER_ADDRESS3);
        err = readDeviceErrorDNR();
    }

    if (err)
    {
        mLastError = WRITE_ERROR_DEVICE_NOT_READY;
        qDebug() << "Device is not ready. Error in: FalconLaserManager::writeDeviceCommand("
                 << b1 << "," << b2 << "," << b3 << "," << b4 << ")";
        return false;
    }

    err = true;
    for (int retry = 0;retry < BUFFER_WRITE_RETRY_COUNT && err;retry++)
    {
        writeDeviceAddress(BUFFER_ADDRESS3);
        writeDeviceByte(b1);
        writeDeviceByte(b2);
        writeDeviceByte(b3);
        writeDeviceByte(b4);
        writeDeviceByte(crc);
        readDeviceErrorCRC();
        err = readDeviceErrorCRC();
    }
    if (err)
    {
        mLastError = WRITE_ERROR_CRC;
        qDebug() << "CRC fault. Error in: FalconLaserManager::writeDeviceCommand("
                 << b1 << "," << b2 << "," << b3 << "," << b4 << ")";
        return false;
    }
    return true;
}

uchar FalconOutputDevice::calcCRC8(uchar val)
{
    uchar TABLE_CRC8[] = {
        0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
        157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
        35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
        190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
        70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
        219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
        101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
        248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
        140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
        17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
        175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
        50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
        202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
        87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
        233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
        116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
    };
    return TABLE_CRC8[val];
}

// virtual interface which is not supported by real device
// actually won't be called, but because of bad architecture should be somehow implemented
bool FalconOutputDevice::moveTo(int x, int y)
{
    return true;
}

bool FalconOutputDevice::lineTo(int x, int y)
{
    return true;
}

bool FalconOutputDevice::cubicTo(int control1X, int control1Y,
                                 int control2X, int control2Y,
                                 int toX, int toY)
{
    return true;
}

bool FalconOutputDevice::conicTo(int controlX, int controlY,
                                 int toX, int toY)
{
    return true;
}

bool FalconOutputDevice::segmentTo(int centerX, int centerY, int toX, int toY)
{
    return true;
}
