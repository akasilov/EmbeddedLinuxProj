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

    const static QString GPIOBASE_DIR_IN;
    const static QString GPIOBASE_DIR_OUT;
    const static QString GPIOBASE_EDGE_NONE;
    const static QString GPIOBASE_EDGE_RISING;
    const static QString GPIOBASE_EDGE_FALLING;
    const static QString GPIOBASE_EDGE_BOTH;

protected:
    bool writeToSysFs(QString FilePath, QString WriteString);
    bool readByteFromSysFs(QString FilePath, quint8 *Value);
    bool isPinExported();

    quint16 pinNumber; //GPIO pin number

    const static QString SYSFS_PATH_STRING;
    const static QString EXPORT_STRING;
    const static QString UNEXPORT_STRING;
    const static QString VALUE_STRING;
    const static QString EDGE_STRING;
    const static QString DIRECTION_STRING;
    const static QString GPIO_STRING;
};

#endif // GPIOBASE_H
