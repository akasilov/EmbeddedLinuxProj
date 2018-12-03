#ifndef I2CDEVICE2_H
#define I2CDEVICE2_H

#include <QString>

class I2CProtocol
{
public:
    I2CProtocol(int busId, quint8 i2cAddress, bool open = true);
    virtual ~I2CProtocol();

    bool writeByte(quint8 byte);
    bool writeBytes(quint8 *buffer, quint8 length);
    quint8 readByte();
    bool readBytes(quint8 *buffer, quint8 length);
    quint8 readRegister(quint8 reg);
    bool writeRegister(quint8 reg, quint8 value);

protected:
    // creates connection to a sensor
    bool openDevice();
    // closes connection to a sensor
    void closeDevice();

private:
    // set slave address
    bool setSlaveAddress(bool force);

private:
    int mDeviceFd;
    quint8 mSlaveAddress;
    QString mFileName;
    QString mDeviceName;
};

#endif // I2CDEVICE2_H
