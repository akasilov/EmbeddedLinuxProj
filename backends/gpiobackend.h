#ifndef BUTTONBACKEND_H
#define BUTTONBACKEND_H

#include "drivers/gpiobutton.h"
#include "drivers/gpioled.h"
#include <QTimer>
#include <QObject>

class GpioBackend : public QObject
{
    Q_OBJECT

public:
    explicit GpioBackend(QObject *parent = nullptr);
    ~GpioBackend();

public slots:
    void pollGpios();

signals:
    void buttonPressedT1();
    void buttonPressedT2();
    void buttonPressedT3();
    void buttonPressedT4();

private:
    gpioButton mT1;
    gpioButton mT2;
    gpioButton mT3;
    gpioButton mT4;
    gpioLed mL1;
    gpioLed mL2;
    gpioLed mL3;
    gpioLed mL4;
    QTimer* mUpdateTimer {nullptr};
};

#endif // BUTTONBACKEND_H
