#include "sensor.h"
#include <QtDebug>

Sensor::Sensor(QString name, I2CProtocol* proto)
    : mSensorName(name)
    , mProto(proto)
{
    qInfo() << "Sensor" << mSensorName << "created";
}

Sensor::~Sensor()
{
    qInfo() << "Sensor" << mSensorName << "destroyed";
}


