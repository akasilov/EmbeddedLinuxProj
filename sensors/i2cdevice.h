#ifndef SENSOR_H
#define SENSOR_H

#include <QString>

class I2CDevice
{
public:
    I2CDevice(QString name, int busId, quint8 i2cAddress);

    // creates connection to a sensor
    bool openDevice();
    bool writeByte(quint8 byte);
    quint8 readByte();

private:
    bool setSlaveAddress(bool force);

private:
    int mDeviceFd;
    quint8 mSlaveAddress;
    QString mFileName;
    QString mDeviceName;
};

#endif // SENSOR_H
