#include <QDebug>

#include <iostream>
#include "i2cdevice.h"

extern "C" {
#include "drivers/i2c-dev.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
}

I2CDevice::I2CDevice(QString name, int busId, quint8 i2cAddress)
    : mSlaveAddress(i2cAddress),
      mFileName(QString("/dev/i2c-") + QString::number(busId)),
      mDeviceName(name)
{
    qDebug() << "Openning file" << mFileName;
    mDeviceFd = open(mFileName.toStdString().c_str(), O_RDWR);

    if (mDeviceFd < 0)
    {
        qCritical() << "Could not open" << mFileName;
        exit(1);
    }
    else
    {
        qInfo() << "Sensor" << mDeviceName << "created";
    }
}

bool I2CDevice::openDevice()
{
    setSlaveAddress(true);

    writeByte(0x04);
    readByte();
    return true;
}

bool I2CDevice::writeByte(quint8 byte)
{
    if (i2c_smbus_write_byte(mDeviceFd, byte) < 0)
    {
        qWarning() << "Write failed";
        return false;
    }
    else
    {
        qDebug() << "Wrote byte" <<  byte;
        return true;
    }
}

quint8 I2CDevice::readByte()
{
    qint32 byte = i2c_smbus_read_byte(mDeviceFd);
    if (byte < 0)
    {
        qWarning() << "Write failed";
        return false;
    }

    qDebug() << "Read byte" <<  byte;
    return static_cast<quint8>(byte);
}

bool I2CDevice::setSlaveAddress(bool force)
{
    /* With force, let the user read from/write to the registers
       even when a driver is also running */
    if (ioctl(mDeviceFd, force ? I2C_SLAVE_FORCE : I2C_SLAVE, mSlaveAddress) < 0)
    {
        qCritical() << "Error: Could not set address to" << mFileName << mSlaveAddress << ":" << strerror(errno);
        return false;
    }
    else
    {
        qInfo() << "Slave address set to" <<  QString::number(mSlaveAddress, 16);
    }
    return true;
}
