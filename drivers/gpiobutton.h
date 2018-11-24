#ifndef GPIOBUTTON_H
#define GPIOBUTTON_H

#include <QString>
#include <QObject>
#include <QTimer>

#include "gpiobase.h"

class gpioButton : public QObject, public gpioBase
{
    Q_OBJECT

public:
    explicit gpioButton(QObject *parent = nullptr, quint16 PinNumber = 0);
    virtual ~gpioButton();

    bool configureGpio(); /* must be called after exportGpio (wait >= 100ms after export call -> udev needs some time to set permissions of exported sysfs folder) */
    uint8_t getValue(); /* read pin value */

    bool pollForInt(); /* checks for pin pressed interrupt and generates onButtonPressed() signal (call this method periodically if onButtonPressed() is used, e.g. every 100-200ms) */

    void setHoldoffTime(int TimeMs); /* set hold off time for onButtonPressed in milliseconds */
    int getHoldoffTime(); /* gets hold off time for onButtonPressed in milliseconds */

    void pollStart(); /* opens gpio pin value file descriptor and initialises for interrupt polling (invoke after configureGpio() and before pollForInterrupt() */
    void pollStop();

signals:
    void onButtonPressed();

private:

    int poll_fd; /* file descriptor */

    QTimer holdoffTimer; /* one shot timer for debouncing gpio pin */
    bool isHoldoffActive; /* signals, if hold off is active (holf off time after gpio falling edge detected) */

    const static quint16 defaultHoldoffTime;

private slots:
    void holdoffTimerExpiredSlot();

};




#endif // GPIOBUTTON_H
