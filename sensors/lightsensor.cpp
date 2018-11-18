#include "lightsensor.h"
#include <QtCore/qmath.h>
#include <QDebug>

LightSensor::LightSensor()
  : I2CDevice("LightSensor", 4, LightSensor::I2C_ADDRESS)
{
    openDevice();
}

double LightSensor::getLightValueInLux()
{
    quint8 mantissa = 0;
    quint8 exponent = 0;

    writeByte(LUX_LOW_BYTE_REG);
    mantissa = 0x0F & readByte();
    writeByte(LUX_HIGH_BYTE_REG);
    quint8 byteHigh = readByte();
    mantissa |= (0x0F & byteHigh) << 4;
    exponent = (byteHigh & 0xF0) >> 4;

    double lux = qPow(2, exponent)*mantissa*0.045;
    qInfo() << "Sensor readings in lux " << lux;

    return lux;
}