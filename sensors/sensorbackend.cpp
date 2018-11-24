#include <QDebug>
#include <QThread>
#include "sensorbackend.h"
#include "colorsensor.h"
#include "lightsensor.h"

SensorBackEnd::SensorBackEnd(QObject *parent) :
    QObject(parent),
    T1(this, 120),
    T2(this, 121),
    updateTimer(new QTimer(this))
{
    emit readingsChanged();

    /* set up timer */
    //updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SIGNAL(readingsChanged()));
    updateTimer->start(500);

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateReading()));

    /*
     **********************************************************
     * Example: set up L1 .. L4 and buttons T1 .. T2     begin
     **********************************************************
    */

    //T1
    connect(&T1, SIGNAL(onButtonPressed()), this, SLOT(buttonT1_event()));
    T1.exportGpio(true);

    //T2
    connect(&T2, SIGNAL(onButtonPressed()), this, SLOT(buttonT2_event()));
    T2.exportGpio(true);

    //L1 .. L4
    gpioLed L1(this, 124);
    gpioLed L2(this, 125);
    gpioLed L3(this, 126);
    gpioLed L4(this, 127);
    L1.exportGpio(true);
    L2.exportGpio(true);
    L3.exportGpio(true);
    L4.exportGpio(true);

    // wait for udev daemon
    QThread::msleep(100);

    T1.configureGpio(); /* configure pin direction and interrupt */
    T2.configureGpio();
    T1.setHoldoffTime(250);
    T2.setHoldoffTime(250);
    T1.pollStart();
    T2.pollStart();

    L1.configureLedGpio();
    L2.configureLedGpio();
    L3.configureLedGpio();
    L4.configureLedGpio();

    L1.setLed(true);
    L2.setLed(true);
    L3.setLed(true);
    L4.setLed(true);
    QThread::msleep(500);
    L1.setLed(false);
    L2.setLed(false);
    L3.setLed(false);
    L4.setLed(false);

    /*
     **********************************************************
     * Example: set up L1 .. L4 and buttons T1 .. T2       end
     **********************************************************
    */


    //updateReading();

    updateTimer->start(200);

}




/* Example: set up buttons T1 .. T2 */
SensorBackEnd::~SensorBackEnd()
{
    T1.pollStop();
    T2.pollStop();
}




QString SensorBackEnd::getReading()
{
    return mSensor->getReadingAsString();
}




/* Example: set up buttons T1 .. T2 */
void SensorBackEnd::buttonT1_event()
{
    static quint32 counter = 0;
    counter++;
    qDebug() << "T1 Event, counter: " << QString::number(counter);
}
/* Example: set up buttons T1 .. T2 */
void SensorBackEnd::buttonT2_event()
{
    static quint32 counter = 0;
    counter++;
    qDebug() << "T2 Event, counter: " << QString::number(counter);
}
/* Example: set up buttons T1 .. T2 */
void SensorBackEnd::updateReading()
{
    /* polling for event */
    T1.pollForInt(); //-> signal onButtonPressed -> slot buttonT1_event()
    T2.pollForInt(); //-> signal onButtonPressed -> slot buttonT2_event()
}





void SensorBackEnd::setSensorType(QString sensorType)
{
    mSensorType = sensorType;
    if (sensorType == "light")
    {
        mSensor.reset(new LightSensor());
    }
    else if (sensorType == "color")
    {
        mSensor.reset(new ColorSensor());
    }
    else
    {
        qWarning() << "Unknown sensor type";
    }
}

QString SensorBackEnd::getSensorType() const
{
    return mSensorType;
}


