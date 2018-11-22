#include <QDebug>
#include "sensorbackend.h"
#include "colorsensor.h"
//#include <QThread>

SensorBackEnd::SensorBackEnd(QObject *parent) :
    QObject(parent),
    mLightValueLux(0.)
{
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateReading()));
    updateTimer->start(500);
    //updateReading();
}

QString SensorBackEnd::getReading()
{
    return QString::number(mLightValueLux);
}

QString SensorBackEnd::getColorReading()
{
    return "Clear: " + QString::number(cData.Clear) + " Red: " + QString::number(cData.Red) + " Green: " + QString::number(cData.Green) + " Blue: " + QString::number(cData.Blue);
}

void SensorBackEnd::updateReading() {
    mLightValueLux = mSensor.getLightInLux();
    //QThread::msleep(5);
    cData = cSensor.getColors(); //memory leak ???
    emit readingsChanged();
}
