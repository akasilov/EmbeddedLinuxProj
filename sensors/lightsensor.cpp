#include "lightsensor.h"
#include "drivers/i2cprotocol.h"
#include <QtCore/qmath.h>
#include <QDebug>


LightSensor::LightSensor()
    : Sensor("LighSensor", new I2CProtocol(4, LightSensor::I2C_ADDRESS))
{}

QString LightSensor::getReadingAsString()
{
    return QString::number(mLightInLux);
}

void LightSensor::readSensorData()
{
    mLightInLux = getLightInLux();
}

float LightSensor::getLightInLux()
{
    quint8 mantissa = 0;
    quint8 exponent = 0;

    // read low byte.
    mProto->writeByte(LUX_LOW_BYTE_REG);
    mantissa = 0x0F & mProto->readByte();

    // read low byte
    mProto->writeByte(LUX_HIGH_BYTE_REG);
    quint8 byteHigh = mProto->readByte();
    mantissa |= (0x0F & byteHigh) << 4;
    exponent = (byteHigh & 0xF0) >> 4;

    float lux = qPow(2, exponent)*mantissa*0.045;
    qDebug() << mSensorName << lux << "lux";

    return lux;
}
