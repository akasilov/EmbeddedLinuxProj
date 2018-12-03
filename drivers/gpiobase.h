#ifndef GPIOBASE_H
#define GPIOBASE_H

#include <QObject>

class gpioBase
{

public:
    gpioBase(quint16 PinNumber);
    virtual ~gpioBase();

    bool exportGpio(bool Export); /* exports or unexports gpio pin (pin must be exported before it can be used) */
    bool setGpioDirection(QString Direction); /* Direction=true -> output; method must be called after exportGpio (wait >= 100ms after export call -> udev needs some time to set permissions of exported sysfs folder) */
    bool setGpioEdge(QString Edge); /* Direction=true -> output; method must be called after exportGpio (wait >= 100ms after export call -> udev needs some time to set permissions of exported sysfs folder) */
    quint8 getValue(); /* read pin value */
    bool setValue(quint8 Value); /* set pin value */
    quint16 getPinNumber(); /* get pin number of current object */

    const QString GPIOBASE_DIR_IN {"in"};
    const QString GPIOBASE_DIR_OUT {"out"};
    const QString GPIOBASE_EDGE_NONE {"none"};
    const QString GPIOBASE_EDGE_RISING {"rising"};
    const QString GPIOBASE_EDGE_FALLING {"falling"};
    const QString GPIOBASE_EDGE_BOTH {"both"};

protected:
    bool writeToSysFs(QString FilePath, QString WriteString);
    bool readByteFromSysFs(QString FilePath, quint8 *Value);
    bool isPinExported();

    quint16 pinNumber; //GPIO pin number

    const QString SYSFS_PATH_STRING {"/sys/class/gpio/"};
    const QString EXPORT_STRING {"export"};
    const QString UNEXPORT_STRING {"unexport"};
    const QString VALUE_STRING {"value"};
    const QString EDGE_STRING {"edge"};
    const QString DIRECTION_STRING {"direction"};
    const QString GPIO_STRING {"gpio"};
};

#endif // GPIOBASE_H
