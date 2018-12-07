#include <QDebug>
#include "sensorbackend.h"
#include "sensors/colorsensor.h"
#include "sensors/lightsensor.h"
#include "sensors/accelsensor.h"
#include "drivers/logger.h"


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
    int timeout = 500;
    if (sensorType == "light")
    {
        timeout = 500;
        mSensor.reset(new LightSensor());
    }
    else if (sensorType == "color")
    {
        timeout = 200;
        mSensor.reset(new ColorSensor());
    }
    else if (sensorType == "acceleration")
    {
        timeout = 10;
        mSensor.reset(new AccelSensor());
    }
    else
    {
        qWarning() << "Unknown sensor type";
        return;
    }

    emit updateReading();

    /* set up timer */
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateReading()));
    mUpdateTimer->start(timeout);
}

QString SensorBackend::getSensorType() const
{
    return mSensorType;
}

void SensorBackend::updateReading()
{
    mSensor->readSensorData();
    emit readingsUpdated();
}

