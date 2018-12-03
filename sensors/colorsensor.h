#ifndef COLORSENSOR_H
#define COLORSENSOR_H

#include "sensor.h"

struct rgbColorAdc
{
    quint16 Clear;
    quint16 Red;
    quint16 Green;
    quint16 Blue;
};

class ColorSensor : public Sensor
{
public:
    ColorSensor();

    virtual QString getReadingAsString() override;
    virtual void readSensorData() override;

private:
    rgbColorAdc mColors;
    rgbColorAdc getColors(void);
    void enableSensor();
    void disableSensor();
    void setIntegrationTime(quint8 value);
    void setRgbcGain(quint8 value);

    void initSensor();
    quint8 readFromRegister(quint8 reg);
    bool writeToRegister(quint8 reg, quint8 value);

    const static int CMD_REPEATED  = 0x80;
    const static int CMD_INCREMENT  = 0xA0;

    const static int ENABLE_SENSOR  = 0x03;
    const static int DISABLE_SENSOR  = 0x00;

    const static int ENABLE_REG  = 0x00;
    const static int ATIME_REG   = 0x01;
    const static int WTIME_REG   = 0x03;
    const static int AILTL_REG   = 0x04;
    const static int AILTH_REG   = 0x05;
    const static int AIHTL_REG   = 0x06;
    const static int AIHTH_REG   = 0x07;
    const static int PERS_REG    = 0x0C;
    const static int CONFIG_REG  = 0x0D;
    const static int CONTROL_REG = 0x0F;
    const static int ID_REG      = 0x12;
    const static int CDATAL_REG  = 0x14;
    const static int CDATAH_REG  = 0x15;
    const static int RDATAL_REG  = 0x16;
    const static int RDATAH_REG  = 0x17;
    const static int GDATAL_REG  = 0x18;
    const static int GDATAH_REG  = 0x19;
    const static int BDATAL_REG  = 0x1A;
    const static int BDATAH_REG  = 0x1B;

    const static int I2C_ADDRESS = 0x29;
};

#endif // COLORSENSOR_H
