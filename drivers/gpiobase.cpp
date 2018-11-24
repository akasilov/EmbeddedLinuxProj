extern "C" {
    #include <stdint.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <string.h>
    #include <stdbool.h>
    #include <errno.h>

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>

    #include <poll.h>
    //#include <unistd.h>
}

#include <QDebug>
#include <QString>
#include <QDir>

#include "gpiobase.h"
#include <QThread>

const QString gpioBase::GPIOBASE_DIR_IN = "in";
const QString gpioBase::GPIOBASE_DIR_OUT = "out";
const QString gpioBase::GPIOBASE_EDGE_NONE = "none";
const QString gpioBase::GPIOBASE_EDGE_RISING = "rising";
const QString gpioBase::GPIOBASE_EDGE_FALLING = "falling";
const QString gpioBase::GPIOBASE_EDGE_BOTH = "both";

const QString gpioBase::SYSFS_PATH_STRING = "/sys/class/gpio/";
const QString gpioBase::EXPORT_STRING = "export";
const QString gpioBase::UNEXPORT_STRING = "unexport";
const QString gpioBase::VALUE_STRING = "value";
const QString gpioBase::EDGE_STRING = "edge";
const QString gpioBase::DIRECTION_STRING = "direction";
const QString gpioBase::GPIO_STRING = "gpio";

gpioBase::gpioBase(quint16 PinNumber):
        pinNumber(PinNumber)
{

}

gpioBase::~gpioBase()
{
    exportGpio(false);
}


/*
 * Exports/Unexports the gpio pin according to PinNumber
 * In case the pin is already exported/unexported, this method
 * takes no further actions.
 *
 * PinNumber: Number of GPIO pin to export/unexport
 * IsExport: True for export, false for unexport
 *
 * return value: un/export successfull
 */
bool gpioBase::exportGpio(bool IsExport)
{
    QString pinname_str = QString::number(pinNumber);
    QString gpiopin_path_str = SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber);
    QString export_path_str = SYSFS_PATH_STRING;
    bool isSuccessful = false;

    /* check if export or unexport */
    if (IsExport == true)
        { export_path_str.append(EXPORT_STRING); }
    else
        { export_path_str.append(UNEXPORT_STRING); }

    /* leave method here if desired state already happend */
    if (isPinExported() == IsExport)
    {
        if (IsExport)
            {qDebug() << gpiopin_path_str << "pin already exported";}
        else
            {qDebug() << gpiopin_path_str << "pin already unexported";}

        return true;
    }

    writeToSysFs(export_path_str, pinname_str);

    /* final debug msg */
    if (isSuccessful == true)
    {
        if (IsExport == true)
            {qDebug() << "export ok";}
        else
            {qDebug() << "unexport ok";}
    }

    return isSuccessful;
}

bool gpioBase::setGpioDirection(QString Direction)
{
    QString *path;

    /* set gpio direction to input */
    path = new QString(SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber) + "/" +DIRECTION_STRING);
    if (writeToSysFs(*path, Direction) == false)
    {
        qWarning() << "could not set" + DIRECTION_STRING;
        return false;
    }
    delete path;

    return true;
}

bool gpioBase::setGpioEdge(QString Edge)
{
    QString *path;

    /* set gpio edge */
    path = new QString(SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber) + "/" + EDGE_STRING);
    if (writeToSysFs(*path, Edge) == false)
    {
        qWarning() << "could not set" << EDGE_STRING;
        return false;
    }
    delete path;

    return true;
}

bool gpioBase::setValue(quint8 Value)
{
    QString *path;
    QString strValue = QString::number(Value);

    /* set gpio pin value */
    path = new QString(SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber) + "/" + VALUE_STRING);
    if (writeToSysFs(*path, strValue) == false)
    {
        qWarning() << "could not set" + VALUE_STRING;
        return false;
    }
    delete path;

    return true;
}

quint16 gpioBase::getPinNumber()
{
    return pinNumber;
}

uint8_t gpioBase::getValue()
{
    quint8 value;
    QString path(SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber) + "/" + VALUE_STRING);

    /* check if pin has been exported on sysfs */
    if (isPinExported() == true)
    {
        /* read gpio value */
        if (readByteFromSysFs(path, &value) == false)
        {
            qWarning() << "could not read gpio" + VALUE_STRING;
        }
        QString s(value);
        return static_cast<quint8>(s.toInt());
    }
    else
    {
        /* pin not exported */
        qWarning() << __FUNCTION__ << "():" << "gpio pin" << pinNumber << "has not been exported";
        return 0;
    }
}

bool gpioBase::writeToSysFs(QString FilePath, QString WriteString)
{
    int fd;

    /* open file */
    qDebug() << "Opening file" << FilePath;
    fd = open(FilePath.toStdString().c_str(), O_WRONLY);
    if (fd < 0)
    {
        qCritical() << "Could not open" << FilePath << "fd" << fd;
        return false;
    }

    /* write to file */
    size_t nbytes = WriteString.toStdString().size(); // write without string terminator
    if (write(fd, WriteString.toStdString().c_str(), nbytes) != static_cast<ssize_t>(nbytes))
    {
        /* write error */
        qCritical() << "error while writing to" << FilePath;
        qDebug() << "string:" << WriteString.toStdString().c_str() << ", size:" << nbytes;
        return false;
    }
    else
    {
        /* write ok */
        //qDebug() << "write ok, string:" << WriteString.toStdString().c_str() << ", size:" << nbytes;
    }

    /* close file */
    if (fd >= 0)
    {
        if (close(fd) != 0)
        {
            return false;
        }
    }
    return true;
}

bool gpioBase::readByteFromSysFs(QString FilePath, quint8 *Value)
{
    int fd;
    //quint8 buffer[sysFsReadBufferLength];

    /* open file */
    qDebug() << "Opening file" << FilePath;
    fd = open(FilePath.toStdString().c_str(), O_RDONLY);
    if (fd < 0)
    {
        qCritical() << "Could not open" << FilePath.toStdString().c_str() << "fd" << fd;
        return false;
    }

    /* read one byte from file */
    int retval = read(fd, Value, 1) != 1;
    if (retval < 0) //read() returns 0, still read 1 byte ???
    {
        /* read error */
        qCritical() << "error while reading from" << FilePath;
        //qDebug() << "read() returned:" << retval;
        return false;
    }
    else
    {
        /* read ok */
        //qDebug() << "read ok, value:" << *Value;
    }

    /* close file */
    if (fd >= 0)
    {
        if (close(fd) != 0)
        {
            return false;
        }
    }
    return true;
}

/*
 * check sysfs folder to see if pin is already exported or not
 *
 * return: true -> exported, false -> not exported
*/
bool gpioBase::isPinExported()
{
    //QString gpiopin_path_str = ;
    QDir dir(SYSFS_PATH_STRING + GPIO_STRING + QString::number(pinNumber));

    return dir.exists();
}
