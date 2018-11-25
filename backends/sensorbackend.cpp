#include <QDebug>
#include "sensorbackend.h"
#include "sensors/colorsensor.h"
#include "sensors/lightsensor.h"

SensorBackend::SensorBackend(QObject *parent) :
    QObject(parent),
    mUpdateTimer(new QTimer(this))
{}

SensorBackend::~SensorBackend()
{}

QString SensorBackend::getReading()
{
    return mSensor->getReadingAsString();
}

void SensorBackend::setSensorType(QString sensorType)
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
        return;
    }

    emit updateReading();

    /* set up timer */
    connect(mUpdateTimer, SIGNAL(timeout()), this, SIGNAL(updateReading()));
    mUpdateTimer->start(500);

}

QString SensorBackend::getSensorType() const
{
    return mSensorType;
}


