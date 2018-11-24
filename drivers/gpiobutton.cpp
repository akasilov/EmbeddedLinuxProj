/*
 * set up gpioButton-Object:
 *  - create instance:
 *      gpioButton Btn(pinnumber);
 *  - export gpio pin in sysfs: call ..
 *      Btn.exportGpio();
 *  - wait for a few hundred ms (udev daemon have to change export folder permissions)
 *  - configure pin (Direction, Edge): call ..
 *      Btn.configureLedGpio();
 *  - get Btn state:
 *      Btn.setLed(true);
 *
 * using with interrupt polling:
 *  - do steps above (whitout Btn.setLed(true))
 *  - connect onButtonPressed() signal:
 *      connect(&Btn, SIGNAL(onButtonPressed()), yourObject, SLOT(yourSlot()));
 *  - set holdoff (timespan where subsequent interrupts are ignored after the first one catched):
 *      Btn.setHoldoffTime(t_ms); //time in milliseconds
 *  - init polling:
 *      Btn.pollStart()
 *  - Invoke periodically (every 100-200ms):
 *      Btn.pollForInt()
 *  - falling edges on pin n now triggers
 *  - finally stop polling:
 *      Btn.pollStop(); //closes file descriptor
 *
 */


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

#include "gpiobutton.h"
#include <QThread>

//const QString gpioButton::SYSFS_PATH_STRING = "/sys/class/gpio/";

const quint16 gpioButton::defaultHoldoffTime = 50;

gpioButton::gpioButton(QObject *parent, quint16 PinNumber)
    : QObject(parent), gpioBase(PinNumber),
    poll_fd(-1),
    holdoffTimer(this),
    isHoldoffActive(false)
{
    /* set up hold off timer */
    holdoffTimer.setSingleShot(true);
    holdoffTimer.setInterval(defaultHoldoffTime);
    holdoffTimer.setTimerType(Qt::CoarseTimer);
    connect(&holdoffTimer, SIGNAL(timeout()), this, SLOT(holdoffTimerExpiredSlot()));
}

gpioButton::~gpioButton()
{
    pollStop();

    /* unexport pin */
    exportGpio(false);

    /* stop timer */
    holdoffTimer.stop();
}

/*
 * before calling this method, you have to invoke pollStart() to open the file descriptor
 * (and finally pollStop() to close the file).
 */
bool gpioButton::pollForInt()
{
    struct pollfd pollfds[] = {{-1, 0, 0}};

    if (poll_fd >= 0)
    {
        //qDebug() << "entersleep -------------------------------------------------------";
        //QThread::msleep(5000);

        //qDebug() << "startpolling -------------------------------------------------------";
        pollfds[0].fd = poll_fd;
        pollfds[0].events = POLLPRI | POLLERR;

        /* poll() for interrupt event without blocking */
        const int timeoutms = 0; /* timeout: 0 -> immediate return; -1 -> block without timeout (library dependent) */
        int retval = poll(pollfds, sizeof(pollfds)/sizeof(pollfds[0]), timeoutms);

        /* reset gpio event flags */
        quint8 dummy;
        read(poll_fd, &dummy, 1);

        /* reset file ptr */
        lseek(poll_fd, 0, SEEK_SET);

        /* check for events */
        if (retval > 0)
        {

            /* event occured - check if its POLLPRI */
            if ((pollfds[0].revents & POLLPRI) != 0)
            {
                /* button interrupt event detected */

                if (isHoldoffActive == false)
                {
                    /* trigger button pressed signal */
                    isHoldoffActive = true; /* activate hold off */
                    holdoffTimer.start(); /* start single shot timer to reset hold off after a given time (debouncing) */
                    emit onButtonPressed();
                    //qDebug() << "poll():  retval:" << retval << " NEW EVENT OCCURED";

                }
                else
                {
                    /* ignore event as hold off is still active (debouncing) */
                    //qDebug() << "poll():  retval:" << retval << " EVENT IGNORED";
                }

            }
        }
        else
        {
            if (retval == 0)
            {
                /* no event occured (timeout) */
                //qDebug() << "poll(): no event";
            }
            else
            {
                if (retval < 0)
                {
                    /* poll() error occured */
                    qCritical() << "error while polling";
                    //qDebug() << "poll(): error";
                    return false;
                }
            }
        }
    }

    return true;
}

void gpioButton::setHoldoffTime(int TimeMs)
{
    holdoffTimer.setInterval(TimeMs);
}

int gpioButton::getHoldoffTime()
{
    return holdoffTimer.interval();
}

bool gpioButton::configureGpio()
{
    //qDebug() << "configure gpio";

    if (setGpioDirection(GPIOBASE_DIR_IN) == true)
    {
        if (setGpioEdge(GPIOBASE_EDGE_FALLING) == true)
        {
            return true;
        }
        else
        {
            qWarning() << "could not set gpio edge pin" << QString::number(pinNumber);
            return false;
        }
    }
    else
    {
        qWarning() << "could not set gpio direction pin" << QString::number(pinNumber);
        return false;
    }
}

uint8_t gpioButton::getValue()
{
    quint8 value;
    QString path(SYSFS_PATH_STRING + "gpio" + QString::number(pinNumber) + "/value");

    /* check if pin has been exported on sysfs */
    if (isPinExported() == true)
    {
        /* read gpio value */
        if (readByteFromSysFs(path, &value) == false)
        {
            qWarning() << "could not read gpio value";
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

void gpioButton::pollStart()
{
    QString path(SYSFS_PATH_STRING + "gpio" + QString::number(pinNumber) + "/value");
    quint8 dummy;

    qDebug() << "pollStart()";

    /* open file */
    poll_fd = open(path.toStdString().c_str(), O_RDONLY);
    if (poll_fd < 0)
    {
        qCritical() << "Could not open" << path << "fd" << poll_fd;
    }
    else
    {
        /* reset gpio event flags */
        read(poll_fd, &dummy, 1);

        /* reset file ptr */
        lseek(poll_fd, 0, SEEK_SET);

        //qDebug() << "pollStart() ok";
    }
}

void gpioButton::pollStop()
{
    qDebug() << "pollStop()";

    /* close export file */
    if (poll_fd >= 0)
    {
        if (close(poll_fd) != 0)
        {
            qCritical() << "could not close pollStop";
        }
        poll_fd = -1;
    }
}

void gpioButton::holdoffTimerExpiredSlot()
{
    isHoldoffActive = false;
    //qDebug() << "hold off timer expired";
}

