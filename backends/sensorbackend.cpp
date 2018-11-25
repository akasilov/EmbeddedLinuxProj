#include <QDebug>
#include "sensorbackend.h"
#include "sensors/colorsensor.h"
#include "sensors/lightsensor.h"
#include "sensors/accelsensor.h"

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
    else if (sensorType == "acceleration")
    {
        mSensor.reset(new AccelSensor());
    }
    else
    {
        qWarning() << "Unknown sensor type";
        return;
    }

    emit updateReading();

    /* set up timer */
    connect(mUpdateTimer, SIGNAL(timeout()), this, SIGNAL(updateReading()));
    mUpdateTimer->start(100);

}

QString SensorBackend::getSensorType() const
{
    return mSensorType;
}


