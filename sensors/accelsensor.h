#ifndef ACCELSENSOR_H
#define ACCELSENSOR_H

#include "sensors/sensor.h"

class AccelSensor : public Sensor
{
public:

    // Set initial input parameters
    enum AccelScale {
        AFS_2G = 0,
        AFS_4G,
        AFS_8G,
        AFS_16G
    };

    AccelSensor(AccelScale scale = AFS_2G);

    virtual QString getReadingAsString() override;
    virtual QVariantList getSensorData() override;

    struct AccelData
    {
        float ax;
        float ay;
        float az;
    };

    enum mGyroScale {
      GFS_250DPS = 0,
      GFS_500DPS,
      GFS_1000DPS,
      GFS_2000DPS
    };

    enum Mscale {
      MFS_14BITS = 0, // 0.6 mG per LSB
      MFS_16BITS      // 0.15 mG per LSB
    };

    virtual void readSensorData() override;

private:
    void initSensor();
    void reset();
    void calibrate();
    void readAccelData(qint16* destination);
    void setResolution();

    uint8_t mAccelScale {AFS_2G};     // AFS_2G, AFS_4G, AFS_8G, AFS_16G
    uint8_t mGyroScale {GFS_250DPS}; // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
    float mResolition; // scale resolutions per LSB for the sensors

    float mGyroBias[3] = {0., 0., 0.};
    float mAccelBias[3] = {0., 0., 0.}; // Bias corrections for gyro and accelerometer
    float mAccelX;
    float mAccelY;
    float mAccelZ; // variables to hold latest sensor data values

    const static int I2C_ADDRESS = 0x68;
    const static int  PWR_MGMT_1      = 0x6B; // Device defaults to the SLEEP mode
    const static int  PWR_MGMT_2      = 0x6C;
    const static int  CONFIG          = 0x1A;
    const static int  SMPLRT_DIV      = 0x19;
    const static int  GYRO_CONFIG     = 0x1B;
    const static int  ACCEL_CONFIG    = 0x1C;
    const static int  ACCEL_CONFIG2   = 0x1D;
    const static int  INT_ENABLE      = 0x38;
    const static int  FIFO_EN         = 0x23;
    const static int  I2C_MST_CTRL    = 0x24;
    const static int  USER_CTRL       = 0x6A;
    const static int  FIFO_COUNTH     = 0x72;
    const static int  FIFO_R_W        = 0x74;
    const static int  XA_OFFSET_H     = 0x77;
    const static int  YA_OFFSET_H     = 0x7A;
    const static int  ZA_OFFSET_H     = 0x7D;
    const static int  ACCEL_XOUT_H    = 0x3B;
};
#endif // ACCELSENSOR_H
