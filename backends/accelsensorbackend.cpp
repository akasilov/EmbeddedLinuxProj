#include "accelsensorbackend.h"
#include <QDebug>
#include "sensors/accelsensor.h"

AccelSensorBackend::AccelSensorBackend(QObject *parent) :
    SensorBackend(parent)
{}

void AccelSensorBackend::updateReading()
{
    mSensor->readSensorData();
    auto data = mSensor->readDataAsFloat();
    x = data[0];
    y = data[1];
    z = data[2];
    emit readingsUpdated();}

float AccelSensorBackend::getX()
{
    return x;
}

float AccelSensorBackend::getY()
{
    return y;
}

float AccelSensorBackend::getZ()
{
    return z;
}


