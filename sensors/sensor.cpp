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

quint8 Sensor::readRegister(quint8 reg)
{
    mProto->writeByte(reg);
    return mProto->readByte();
}

bool Sensor::writeRegister(quint8 reg, quint8 value)
{
    bool b = mProto->writeByte(reg); /* CMD: desired register */
    if (b == true)
    {
        b = mProto->writeByte(value); /* write to selected register */
    }
    return b;
}
