#include "accelsensor.h"

AccelSensor::AccelSensor()
  : I2CDevice("Acceleration", 4, AccelSensor::I2C_ADDRESS)
{
    openDevice();
}

int AccelSensor::getPosition()
{
    return mX;
}
