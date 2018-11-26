#include "accelsensorbackend.h"
#include <QDebug>
#include "sensors/accelsensor.h"

AccelSensorBackend::AccelSensorBackend(QObject *parent) :
    SensorBackend(parent)
{}

float AccelSensorBackend::getX()
{
    return mSensor->getX();
}

float AccelSensorBackend::getY()
{
    return mSensor->getY();
}

float AccelSensorBackend::getZ()
{
    return mSensor->getZ();
}


