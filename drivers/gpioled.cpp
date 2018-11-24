/*
 * set up gpioLed-Object:
 *  - create instance:
 *      gpioLed LEDn(pinnumber);
 *  - export gpio pin in sysfs:
 *      LEDn.exportGpio();
 *  - wait for a few hundred ms (udev deamon have to change rights of export folder)
 *  - configure pin (pin direction):
 *      LEDn.configureLedGpio
 *  - set LED state:
 *      LEDn.setLed(true);
 */

#include "gpioled.h"
#include <QDebug>

gpioLed::gpioLed(QObject *parent, quint16 PinNumber)
    : QObject(parent), gpioBase(PinNumber)
{

}

gpioLed::~gpioLed()
{

}

bool gpioLed::getLed()
{
    if (isPinExported() == true)
    {
        if (getValue() == 0)
        {
            return false;
        }
        else
        {
            return true;
        }

    }
    else
    {
        qWarning() << "Pin" + QString::number(pinNumber) + "is not exported !";
        return false;
    }
}

bool gpioLed::configureLedGpio()
{
    return setGpioDirection(GPIOBASE_DIR_OUT);
}

void gpioLed::setLed(bool isOn)
{
    if (isPinExported() == true)
    {
        if (isOn == true)
        {
            setValue(1);
        }
        else
        {
            setValue(0);
        }
    }
}
