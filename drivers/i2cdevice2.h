#ifndef I2CDEVICE2_H
#define I2CDEVICE2_H

#include <QString>

class I2CDevice2
{
public:
    I2CDevice2(QString name, int busId, quint8 i2cAddress);
    virtual ~I2CDevice2();

    // creates connection to a sensor
    bool openDevice();
    void closeDevice();
    bool writeByte(quint8 byte);
    bool writeBytes(quint8 *buffer, quint8 length);
    quint8 readByte();
    bool readBytes(quint8 *buffer, quint8 length);

private:
    bool setSlaveAddress(bool force);

private:
    int mDeviceFd;
    quint8 mSlaveAddress;
    QString mFileName;
    QString mDeviceName;
};

#endif // I2CDEVICE2_H
