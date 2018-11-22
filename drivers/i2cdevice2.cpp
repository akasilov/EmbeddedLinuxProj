#include <QDebug>
#include <iostream>
#include "i2cdevice2.h"

extern "C" {
//#include "i2c-dev.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
}

I2CDevice2::I2CDevice2(QString name, int busId, quint8 i2cAddress)
    : mSlaveAddress(i2cAddress),
      mFileName(QString("/dev/i2c-") + QString::number(busId)),
      mDeviceName(name)
{
    qDebug() << "Opening file" << mFileName;
    mDeviceFd = open(mFileName.toStdString().c_str(), O_RDWR);

    if (mDeviceFd < 0)
    {
        qCritical() << "Could not open" << mFileName;
        exit(1);
    }
    else
    {
        qInfo() << "Sensor" << mDeviceName << "opened";
    }
}

I2CDevice2::~I2CDevice2()
{
    closeDevice();
}

bool I2CDevice2::openDevice()
{
    return setSlaveAddress(true);
}

void I2CDevice2::closeDevice()
{
    close(mDeviceFd);
    qInfo() << "Sensor" << mDeviceName << "closed";
}

bool I2CDevice2::writeByte(quint8 byte)
{
    quint8 buffer[1] = {byte};
    return writeBytes(buffer, 1);
}

bool I2CDevice2::writeBytes(quint8 *buffer, quint8 length)
{
    if (buffer == Q_NULLPTR)
    {
        qWarning() << "buffer null pointer in function " << __FUNCTION__;
        return false;
    }

    if (write(mDeviceFd, buffer, length) != length)
    {
        /* write error */
        qWarning() << "Write failed";
        return false;
    }
    else
    {
        /* write successful */
        qDebug() << "Wrote" << length << "bytes";
        return true;
    }
}

quint8 I2CDevice2::readByte()
{
    quint8 buffer[1] = {0};

    readBytes(static_cast<quint8*>(buffer), 1);

    return static_cast<quint8>(buffer[0]);
}

bool I2CDevice2::readBytes(quint8 *buffer, quint8 length)
{
    if (buffer == Q_NULLPTR)
    {
        qWarning() << "buffer null pointer in function " << __FUNCTION__;
        return false;
    }

    if (read(mDeviceFd, buffer, length) != length)
    {
        /* write error */
        qWarning() << "Read failed";
        return false;
    }
    else
    {
        /* write successful */
        qDebug() << "Read" << length << "bytes";
        return true;
    }
}

bool I2CDevice2::setSlaveAddress(bool force)
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
