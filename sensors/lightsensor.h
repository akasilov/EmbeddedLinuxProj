#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "drivers/i2cdevice.h"

class LightSensor : public I2CDevice
{
public:
    LightSensor();

    double getLightValueInLux();

private:
    const static int LUX_LOW_BYTE_REG = 0x04;
    const static int LUX_HIGH_BYTE_REG = 0x03;
    const static int I2C_ADDRESS = 0x4A;
};

#endif // LIGHTSENSOR_H
