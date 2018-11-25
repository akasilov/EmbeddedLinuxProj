#include "accelsensorbackend.h"
#include <QDebug>
#include "sensors/accelsensor.h"

AccelSensorBackend::AccelSensorBackend(QObject *parent) :
    SensorBackend(parent)
{}

float AccelSensorBackend::getX()
{
    qDebug() << "getX" << mSensor->getY();
    return mSensor->getX();
}

float AccelSensorBackend::getY()
{
    qDebug() << "getY" << mSensor->getY();
    return mSensor->getY();
}

float AccelSensorBackend::getZ()
{
    qDebug() << "getZ" << mSensor->getY();
    return mSensor->getZ();
}


