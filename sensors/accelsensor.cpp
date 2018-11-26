#include <QDebug>
#include <QThread>
#include "accelsensor.h"

AccelSensor::AccelSensor()
    : Sensor("AccelSensor", new I2CProtocol(4, AccelSensor::I2C_ADDRESS))
{
    reset(); // Reset registers to default in preparation for device calibration

    calibrate(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
    //qInfo() << "x gyro bias" << gyroBias[0];
    //qInfo() << "y gyro bias" << gyroBias[1];
    //qInfo() << "z gyro bias" << gyroBias[2];
    qInfo() << "x accel bias" << accelBias[0];
    qInfo() << "y accel bias" << accelBias[1];
    qInfo() << "z accel bias" << accelBias[2];
    QThread::sleep(2);
    initSensor();
    getAres();
}

QString AccelSensor::getReadingAsString()
{
    readAccelData(accelCount);  // Read the x/y/z adc values
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];
    az = (float)accelCount[2]*aRes - accelBias[2];
    qDebug() << mSensorName << "ax: " << az << " ay: " << ay << " az: " << az;
    return QString("X: ") + QString::number(ax) + " Y: " + QString::number(ay) + " Z: " + QString::number(az);
}

float AccelSensor::getX()
{
    return ax;
}

float AccelSensor::getY()
{
    return ay;
}

float AccelSensor::getZ()
{
    return ax;
}

void AccelSensor::initSensor()
{
    // Initialize MPU9250 device
    // wake up device
     writeRegister(PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
     QThread::msleep(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

    // get stable time source
     writeRegister(PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
     writeRegister(CONFIG, 0x03);

    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
     writeRegister(SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
     uint8_t c = readRegister(GYRO_CONFIG); // get current GYRO_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
     c = c & ~0x02; // Clear Fchoice bits [1:0]
     c = c & ~0x18; // Clear AFS bits [4:3]
     c = c | static_cast<uint8_t>(Gscale << 3); // Set full scale range for the gyro
    // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
     writeRegister(GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register

    // Set accelerometer full-scale range configuration
     c = readRegister(ACCEL_CONFIG); // get current ACCEL_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
     c = c & ~0x18;  // Clear AFS bits [4:3]
     c = c | static_cast<uint8_t>(Ascale << 3); // Set full scale range for the accelerometer
     writeRegister(ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
     c = readRegister(ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
     c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
     c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
     writeRegister(ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value

    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
    // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

     // Configure Interrupts and Bypass Enable
     // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips
     // can join the I2C bus and all can be controlled by the Arduino as master
     //writeRegister(INT_PIN_CFG, 0x22);
     //writeRegister(INT_ENABLE, 0x01); // Enable data ready (bit 0) interrupt
}

void AccelSensor::reset()
{
    // reset device
    writeRegister(PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    QThread::msleep(100);
}

void AccelSensor::calibrate(float * dest1, float * dest2)
{
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count, fifo_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

    // reset device, reset all registers, clear gyro and accelerometer bias registers
    writeRegister(PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    QThread::msleep(100);

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    writeRegister(PWR_MGMT_1, 0x01);
    writeRegister(PWR_MGMT_2, 0x00);
    QThread::msleep(200);

    // Configure device for bias calculation
    writeRegister(INT_ENABLE, 0x00);   // Disable all interrupts
    writeRegister(FIFO_EN, 0x00);      // Disable FIFO
    writeRegister(PWR_MGMT_1, 0x00);   // Turn on internal clock source
    writeRegister(I2C_MST_CTRL, 0x00); // Disable I2C master
    writeRegister(USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    writeRegister(USER_CTRL, 0x0C);    // Reset FIFO and DMP
    QThread::msleep(15);

    // Configure MPU9250 gyro and accelerometer for bias calculation
    writeRegister(CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    writeRegister(SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    writeRegister(GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    writeRegister(ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    writeRegister(USER_CTRL, 0x40);   // Enable FIFO
    writeRegister(FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    QThread::msleep(40); // accumulate 40 samples in 80 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    writeRegister(FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
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

    /// Push gyro biases to hardware registers
    /*  writeRegister(XG_OFFSET_H, data[0]);
     writeRegister(XG_OFFSET_L, data[1]);
     writeRegister(YG_OFFSET_H, data[2]);
     writeRegister(YG_OFFSET_L, data[3]);
     writeRegister(ZG_OFFSET_H, data[4]);
     writeRegister(ZG_OFFSET_L, data[5]);
   */
    dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
    dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

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

    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
    /*  writeRegister(XA_OFFSET_H, data[0]);
     writeRegister(XA_OFFSET_L, data[1]);
     writeRegister(YA_OFFSET_H, data[2]);
     writeRegister(YA_OFFSET_L, data[3]);
     writeRegister(ZA_OFFSET_H, data[4]);
     writeRegister(ZA_OFFSET_L, data[5]);
   */
    // Output scaled accelerometer biases for manual subtraction in the main program
    dest2[0] = static_cast<float>(accel_bias[0])/static_cast<float>(accelsensitivity);
    dest2[1] = static_cast<float>(accel_bias[1])/static_cast<float>(accelsensitivity);
    dest2[2] = static_cast<float>(accel_bias[2])/static_cast<float>(accelsensitivity);
}

void AccelSensor::getAres() {
    switch (Ascale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
        aRes = 2.0/32768.0;
        break;
    case AFS_4G:
        aRes = 4.0/32768.0;
        break;
    case AFS_8G:
        aRes = 8.0/32768.0;
        break;
    case AFS_16G:
        aRes = 16.0/32768.0;
        break;
    }
    qInfo() << "Accelerometer sensitivity is" << 1.0f/aRes <<  "LSB/g";
}

void AccelSensor::readAccelData(int16_t * destination)
{
    uint8_t rawData[6];  // x/y/z accel register data stored here
    mProto->writeByte(ACCEL_XOUT_H);
    mProto->readBytes(&rawData[0], 6);  // Read the six raw data registers into data array
    destination[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    destination[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
}
