#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "sensor.h"

class LightSensor : public Sensor
{
public:
    LightSensor();

    virtual QString getReadingAsString() override;

private:
    double getLightInLux();

    const static int LUX_LOW_BYTE_REG = 0x04;
    const static int LUX_HIGH_BYTE_REG = 0x03;
    const static int I2C_ADDRESS = 0x4A;
};

#endif // LIGHTSENSOR_H
