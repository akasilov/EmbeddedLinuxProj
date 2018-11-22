#ifndef ACCELSENSOR_H
#define ACCELSENSOR_H

#include "drivers/i2cdevice2.h"

class AccelSensor: public I2CDevice2
{
public:
    AccelSensor();

    int getPosition();
private:
    const static int I2C_ADDRESS = 0x68;

    int mX;
    int mY;
    int mZ;
};

#endif // ACCELSENSOR_H
