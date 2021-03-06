#include <QDebug>
#include <QThread>
#include "accelsensor.h"


AccelSensor::AccelSensor(AccelScale scale)
    : Sensor("AccelSensor", new I2CProtocol(4, AccelSensor::I2C_ADDRESS))
    , mAccelScale(scale)
{
    setResolution();
    reset(); // Reset registers to default in preparation for device calibration

    calibrate(); // Calibrate gyro and accelerometers, load biases in bias registers
    qDebug() << "x accel bias" << mAccelBias[0];
    qDebug() << "y accel bias" << mAccelBias[1];
    qDebug() << "z accel bias" << mAccelBias[2];
    QThread::sleep(2);
    initSensor();
}

QString AccelSensor::getReadingAsString()
{
    return QString("X: ") + QString::number(mAccelX) + " Y: " + QString::number(mAccelY) + " Z: " + QString::number(mAccelZ);
}

QVariantList AccelSensor::getSensorData()
{
    return QVariantList {mAccelX, mAccelY, mAccelZ};
}

void AccelSensor::readSensorData()
{
    qint16 accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
    readAccelData(accelCount);  // Read the x/y/z adc values
    mAccelX = static_cast<float>(accelCount[0])*mResolition - mAccelBias[0];  // get actual g value, this depends on scale being set
    mAccelY = static_cast<float>(accelCount[1])*mResolition - mAccelBias[1];
    mAccelZ = static_cast<float>(accelCount[2])*mResolition - mAccelBias[2];
    qDebug() << mSensorName << "ax: " << mAccelZ << " ay: " << mAccelY << " az: " << mAccelZ;
}

void AccelSensor::initSensor()
{
    // Initialize MPU9250 device
    // wake up device
     mProto->writeRegister(PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
     QThread::msleep(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

    // get stable time source
     mProto->writeRegister(PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
     mProto->writeRegister(CONFIG, 0x03);

    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
     mProto->writeRegister(SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
     uint8_t c = mProto->readRegister(GYRO_CONFIG); // get current GYRO_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
     c = c & ~0x02; // Clear Fchoice bits [1:0]
     c = c & ~0x18; // Clear AFS bits [4:3]
     c = c | static_cast<uint8_t>(mGyroScale << 3); // Set full scale range for the gyro
    // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
     mProto->writeRegister(GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register

    // Set accelerometer full-scale range configuration
     c = mProto->readRegister(ACCEL_CONFIG); // get current ACCEL_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
     c = c & ~0x18;  // Clear AFS bits [4:3]
     c = c | static_cast<uint8_t>(mAccelScale << 3); // Set full scale range for the accelerometer
     mProto->writeRegister(ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
     c = mProto->readRegister(ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
     c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
     c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
     mProto->writeRegister(ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value
}

void AccelSensor::reset()
{
    // reset device
    mProto->writeRegister(PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    QThread::msleep(100);
}

void AccelSensor::calibrate()
{
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count, fifo_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

    // reset device, reset all registers, clear gyro and accelerometer bias registers
    mProto->writeRegister(PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    QThread::msleep(100);

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    mProto->writeRegister(PWR_MGMT_1, 0x01);
    mProto->writeRegister(PWR_MGMT_2, 0x00);
    QThread::msleep(200);

    // Configure device for bias calculation
    mProto->writeRegister(INT_ENABLE, 0x00);   // Disable all interrupts
    mProto->writeRegister(FIFO_EN, 0x00);      // Disable FIFO
    mProto->writeRegister(PWR_MGMT_1, 0x00);   // Turn on internal clock source
    mProto->writeRegister(I2C_MST_CTRL, 0x00); // Disable I2C master
    mProto->writeRegister(USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    mProto->writeRegister(USER_CTRL, 0x0C);    // Reset FIFO and DMP
    QThread::msleep(15);

    // Configure MPU9250 gyro and accelerometer for bias calculation
    mProto->writeRegister(CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    mProto->writeRegister(SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    mProto->writeRegister(GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    mProto->writeRegister(ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    mProto->writeRegister(USER_CTRL, 0x40);   // Enable FIFO
    mProto->writeRegister(FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    QThread::msleep(40); // accumulate 40 samples in 80 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    mProto->writeRegister(FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    mProto->writeByte(FIFO_COUNTH); // read FIFO sample count
    mProto->readBytes(&data[0], 2); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (ii = 0; ii < packet_count; ii++) {
        int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
        mProto->writeByte(FIFO_R_W);
        mProto->readBytes(&data[0], 12); // read data for averaging
        accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
        accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
        accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
        gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
        gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
        gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

        accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];
        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];

    }
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
    else {accel_bias[2] += (int32_t) accelsensitivity;}

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    mGyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
    mGyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    mGyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    mProto->writeByte(XA_OFFSET_H);
    mProto->readBytes(&data[0], 2); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    mProto->writeByte(YA_OFFSET_H);
    mProto->readBytes(&data[0], 2);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    mProto->writeByte(ZA_OFFSET_H);
    mProto->readBytes(&data[0], 2);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(ii = 0; ii < 3; ii++) {
        if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Output scaled accelerometer biases for manual subtraction in the main program
    mAccelBias[0] = static_cast<float>(accel_bias[0])/static_cast<float>(accelsensitivity);
    mAccelBias[1] = static_cast<float>(accel_bias[1])/static_cast<float>(accelsensitivity);
    mAccelBias[2] = static_cast<float>(accel_bias[2])/static_cast<float>(accelsensitivity);
}

void AccelSensor::setResolution() {
    switch (mAccelScale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
        mResolition = 2.0/32768.0;
        break;
    case AFS_4G:
        mResolition = 4.0/32768.0;
        break;
    case AFS_8G:
        mResolition = 8.0/32768.0;
        break;
    case AFS_16G:
        mResolition = 16.0/32768.0;
        break;
    }
}

void AccelSensor::readAccelData(qint16 * destination)
{
    uint8_t rawData[6];  // x/y/z accel register data stored here
    mProto->writeByte(ACCEL_XOUT_H);
    mProto->readBytes(&rawData[0], 6);  // Read the six raw data registers into data array
    destination[0] = (qint16)(((qint16)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = (qint16)(((qint16)rawData[2] << 8) | rawData[3]) ;
    destination[2] = (qint16)(((qint16)rawData[4] << 8) | rawData[5]) ;
}
