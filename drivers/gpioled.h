#ifndef LED_H
#define LED_H

#include <QObject>
#include "gpiobase.h"

class gpioLed : public QObject, public gpioBase
{
    Q_OBJECT

public:
    explicit gpioLed(QObject *parent = nullptr, quint16 PinNumber = 0);
    ~gpioLed();

    void setLed(bool isOn); /* enable LED */
    bool getLed(); /* get LED state (On/Off) */
    bool configureLedGpio(); /* configure current pin numbers gpio port */

signals:

public slots:
};

#endif // LED_H
