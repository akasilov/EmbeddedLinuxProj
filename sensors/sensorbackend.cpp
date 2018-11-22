#include <QDebug>
#include "sensorbackend.h"
#include "colorsensor.h"
#include "lightsensor.h"

SensorBackEnd::SensorBackEnd(QObject *parent) :
    QObject(parent),
    updateTimer(new QTimer(this))
{
    emit readingsChanged();
    //updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SIGNAL(readingsChanged()));
    updateTimer->start(500);
}

QString SensorBackEnd::getReading()
{
    return mSensor->getReadingAsString();
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


