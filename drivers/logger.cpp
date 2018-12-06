
/*
 *
 *
 *
 *
 *
 *
*/



#include "logger.h"

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

    //#include <termios.h>
}

#include <QDebug>


const QString Logger::DEFAULT_UART_DEVICE_STRING = "/dev/ttyS1";
const speed_t Logger::DEFAULT_BAUDRATE = B115200;

/*
 * Baud Rates
 *
 B57600
 B115200
 B230400
 B460800
 B500000
 B576000
 B921600
 B1000000
 B1152000
 B1500000
 B2000000
 B2500000
 B3000000
 B3500000
 B4000000
*/

Logger::Logger(QObject *parent)
    : QObject(parent),
      isFileOpen(false)
{
    openSerialConnection(DEFAULT_UART_DEVICE_STRING, DEFAULT_BAUDRATE); //open serial connection with 115'200 baud
}

Logger::Logger(QObject *parent, QString SerialDevicePath, speed_t Baudrate)
    : QObject(parent),
      isFileOpen(false)
{
    openSerialConnection(SerialDevicePath, Baudrate); //open serial connection with 115'200 baud
}

Logger::~Logger()
{
    if (isFileOpen == true)
    {
        closeSerialConnection();
    }
}

bool Logger::openSerialConnection(QString DevicePath, speed_t BaudRate)
{
    struct termios settings;
    int retval;

    /* check if already one is open */
    if (isConnectionOpen() == true)
    {
        closeSerialConnection();
    }

    /* open file */
    fd = open(DevicePath.toStdString().c_str(), O_RDWR);
    if (fd < 0)
    {
        qWarning() << "uart: could not open" << DevicePath.toStdString().c_str() << "fd" << fd;
        return false;
    }

    /* get current serial port configuration */
    retval = tcgetattr(fd, &settings);
    if (retval < 0)
    {
        qWarning() << "uart: could not get termios attributes, return value:" << retval;
        return false;
    }

    /* configure serial port */
    retval = cfsetospeed(&settings, BaudRate); // set baud rate
    if (retval < 0)
    {
        qWarning() << "uart: could not set baud rate, return value:" << retval;
        return false;
    }
    settings.c_cflag &= static_cast<unsigned int>(~PARENB); // no parity
    settings.c_cflag &= static_cast<unsigned int>(~CSTOPB); // 1 stop bit
    settings.c_cflag &= static_cast<unsigned int>(~CSIZE); // no character size mask
    settings.c_cflag |= CS8 | CLOCAL; // 8 bits
    settings.c_oflag &= static_cast<unsigned int>(~OPOST); /* raw output */
    settings.c_lflag = ICANON; // canonical mode
    tcsetattr(fd, TCSANOW, &settings); // apply the settings
    if (retval < 0)
    {
        qWarning() << "uart: could not set termios attributes, return value:" << retval;
        return false;
    }
    tcflush(fd, TCOFLUSH);
    if (retval < 0)
    {
        qWarning() << "uart: could not flush, return value:" << retval;
        return false;
    }

    //qDebug() << "File" << DevicePath << "opened, file descriptor nr" << fd;
    isFileOpen = true;
    return true;
}

bool Logger::closeSerialConnection()
{
    /* close file */
    if (fd >= 0)
    {
        if (close(fd) != 0)
        {
            qWarning() << "error on closing file descriptor" << fd;
            return false;
        }
    }

    //qDebug() << "file descriptor" << fd << "closed";
    isFileOpen = false;
    return true;
}

bool Logger::isConnectionOpen()
{
    return isFileOpen;
}

bool Logger::sendString(QString message)
{
    ssize_t retval;
    size_t bytesToWrite;

    bytesToWrite = message.toStdString().size();

    if (Logger::isConnectionOpen() == true)
    {
        retval = write(fd, message.toStdString().c_str(), bytesToWrite);
        if (retval == static_cast<ssize_t>(bytesToWrite))
        {
            /* write ok */
            //qDebug() << "write to" << UART_DEVICE_STRING << "ok";
        }
        else
        {
            /* write error */
            qWarning() << "Write error, fd:" << fd;
            //qDebug() << "retval" << retval << "!=" << "bytesToWrite" << bytesToWrite;
            closeSerialConnection();
            return false;
        }
    }
    else
    {
        qWarning() << "uart: device not open";
    }

    return true;
}
