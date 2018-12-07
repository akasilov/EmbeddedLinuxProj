#include "gpiobackend.h"
#include <QThread>
#include <QDebug>

GpioBackend::GpioBackend(QObject *parent) :
    QObject(parent),
    mT1(this, 120),
    mT2(this, 121),
    mT3(this, 122),
    mT4(this, 123),
    mL1(this, 124),
    mL2(this, 125),
    mL3(this, 126),
    mL4(this, 127),
    mUpdateTimer(new QTimer(this))
{

    //T1
    connect(&mT1, SIGNAL(onButtonPressed()), this, SIGNAL(buttonPressedT1()));
    mT1.exportGpio(true);

    //T2
    connect(&mT2, SIGNAL(onButtonPressed()), this, SIGNAL(buttonPressedT2()));
    mT2.exportGpio(true);

    //T3
    connect(&mT3, SIGNAL(onButtonPressed()), this, SIGNAL(buttonPressedT3()));
    mT3.exportGpio(true);

    //T3
    connect(&mT4, SIGNAL(onButtonPressed()), this, SIGNAL(buttonPressedT4()));
    mT4.exportGpio(true);

    //L1 .. L4
    mL1.exportGpio(true);
    mL2.exportGpio(true);
    mL3.exportGpio(true);
    mL4.exportGpio(true);

    // wait for udev daemon
    QThread::msleep(100);

    mT1.configureGpio(); /* configure pin direction and interrupt */
    mT2.configureGpio();
    mT3.configureGpio();
    mT4.configureGpio();
    mT1.setHoldoffTime(250);
    mT2.setHoldoffTime(250);
    mT3.setHoldoffTime(250);
    mT4.setHoldoffTime(250);
    mT1.pollStart();
    mT2.pollStart();
    mT3.pollStart();
    mT4.pollStart();

    mL1.configureLedGpio();
    mL2.configureLedGpio();
    mL3.configureLedGpio();
    mL4.configureLedGpio();

    mL1.setLed(true);
    mL2.setLed(true);
    mL3.setLed(true);
    mL4.setLed(true);
    QThread::msleep(500);
    mL1.setLed(false);
    mL2.setLed(false);
    mL3.setLed(false);
    mL4.setLed(false);

    /*
     **********************************************************
     * Example: set up L1 .. L4 and buttons T1 .. T2       end
     **********************************************************
    */
    connect(mUpdateTimer, SIGNAL(timeout()), this, SIGNAL(updateTimerExpired()));
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(pollGpios()));
    mUpdateTimer->start(200);
}

GpioBackend::~GpioBackend()
{
    mT1.pollStop();
    mT2.pollStop();
    mT3.pollStop();
}

void GpioBackend::pollGpios()
{
    /* polling for event */
    mT1.pollForInt(); //-> signal onButtonPressed -> slot buttonPressedT1()
    mT2.pollForInt(); //-> signal onButtonPressed -> slot buttonPressedT2()
    mT3.pollForInt(); //-> signal onButtonPressed -> slot buttonPressedT3()
    mT4.pollForInt(); //-> signal onButtonPressed -> slot buttonPressedT4()
}

void GpioBackend::setLedL1(bool isSet)
{
    mL1.setLed(isSet);
}

void GpioBackend::setLedL2(bool isSet)
{
    mL2.setLed(isSet);
}

void GpioBackend::setLedL3(bool isSet)
{
    mL3.setLed(isSet);
}

void GpioBackend::setLedL4(bool isSet)
{
    mL4.setLed(isSet);
}

