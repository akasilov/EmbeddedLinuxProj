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

    #include <poll.h>
}

#include <QDebug>


const QString Logger::UART_DEVICE_STRING = "/dev/ttyFIQ0";


Logger::Logger(QObject *parent) : QObject(parent)
{

}


bool Logger::sendString(QString message)
{
    ssize_t retval;
    size_t bytesToWrite;

    bytesToWrite = message.toStdString().size();

    if (openDevice() == true)
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
            qWarning() << "Write error, device: " << UART_DEVICE_STRING << "fd:" << fd;
            qDebug() << "retval" << retval << "!=" << "bytesToWrite" << bytesToWrite;
            closeDevice();
            return false;
        }

        closeDevice();
    }

    return true;
}

bool Logger::openDevice()
{
    /* open file */
    fd = open(UART_DEVICE_STRING.toStdString().c_str(), O_WRONLY);
    if (fd < 0)
    {
        qCritical() << "Could not open" << UART_DEVICE_STRING.toStdString().c_str() << "fd" << fd;
        return false;
    }

    qDebug() << "File" << UART_DEVICE_STRING << "opened, file descriptor nr" << fd;
    return true;
}

bool Logger::closeDevice()
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

    qDebug() << "File descriptor" << fd << "closed";
    return true;
}
