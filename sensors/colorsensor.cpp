#include "colorsensor.h"
#include "drivers/i2cprotocol.h"
#include <QDebug>

ColorSensor::ColorSensor()
    : Sensor("ColorSensor", new I2CProtocol(I2C_BUS_ID, ColorSensor::I2C_ADDRESS))
{
    initSensor();
}

QString ColorSensor::getReadingAsString()
{
    return "Clear: " + QString::number(mColors.Clear, 'g', 6) + " Red: " + QString::number(mColors.Red) + " Green: " + QString::number(mColors.Green) + " Blue: " + QString::number(mColors.Blue);
}

void ColorSensor::readSensorData()
{
    quint8 delta = 10;

    mColors = getColors();

    /* auto adjust integration time */
    if (mColors.Clear < 0.1)
    {
        if (static_cast<qint16>(currentIntegrationTime) - delta > 0)
        {
            currentIntegrationTime -= delta;
            setIntegrationTime(currentIntegrationTime);
            qWarning() << "setIntegrationTime:" << currentIntegrationTime;
        }
    }
    else if (mColors.Clear > 0.9)
    {
        if (static_cast<qint16>(currentIntegrationTime) + delta < 255)
        {
            currentIntegrationTime += delta;
            setIntegrationTime(currentIntegrationTime);
            qWarning() << "setIntegrationTime:" << currentIntegrationTime;
        }
    }
}

QVariantList ColorSensor::getSensorData() {
    return QVariantList {mColors.Clear, mColors.Red, mColors.Green, mColors.Blue};
}

/* read out color data from sensor */
rgbColor ColorSensor::getColors()
{
    rgbColor dataf = {0.0, 0.0, 0.0, 0.0};
    struct
    {
        quint16 Clear;
        quint16 Red;
        quint16 Green;
        quint16 Blue;
    } data = {0, 0, 0, 0};

    const quint8 length = 8;
    quint8 buffer[length];
    bool isOk = false;

    /* read all 8 color registers at once (Clear, Red, Green, Blue; high and low byte) */
    isOk = writeToRegister(CDATAL_REG, CMD_INCREMENT | CDATAL_REG); //set adress to first CRGB register, use increment mode (inc. addr. every reg. read)
    if (isOk == true)
    {
        isOk = mProto->readBytes(buffer, length); //read all values
    }
    if (isOk == true)
    {
        /* Clear */
        data.Clear = buffer[0];
        data.Clear |= static_cast<quint16>(buffer[1]) << 8;
        /* Red */
        data.Red = buffer[2];
        data.Red |= static_cast<quint16>(buffer[3]) << 8;
        /* Green */
        data.Green = buffer[4];
        data.Green |= static_cast<quint16>(buffer[5]) << 8;
        /* Blue */
        data.Blue = buffer[6];
        data.Blue |= static_cast<quint16>(buffer[7]) << 8;

        /* calculate relative values */
        dataf.Clear = static_cast<double>(data.Clear) / ADC_MAXVALUE;
        dataf.Red = static_cast<double>(data.Red) / ADC_MAXVALUE;
        dataf.Green = static_cast<double>(data.Green) / ADC_MAXVALUE;
        dataf.Blue = static_cast<double>(data.Blue) / ADC_MAXVALUE;
    }
    if (isOk == true)
    {
        qDebug() << mSensorName << "clear: " << data.Clear << " red: " << data.Red << " green: " << data.Green << " blue: " << data.Blue;
    }
    else
    {
        qWarning() << "could not read color sensor data in file " << __FILE__ << " function " << __FUNCTION__ << " line " << __LINE__;

    }

    return dataf;
}

void ColorSensor::enableSensor()
{
    writeToRegister(ENABLE_REG, ENABLE_SENSOR);
}

void ColorSensor::disableSensor()
{
    writeToRegister(ENABLE_REG, DISABLE_SENSOR);
}

void ColorSensor::setIntegrationTime(quint8 value)
{
    currentIntegrationTime = value;
    writeToRegister(ATIME_REG, currentIntegrationTime);
}

void ColorSensor::setRgbcGain(quint8 value)
{
    currentRgbGain = value;
    writeToRegister(CONTROL_REG, currentRgbGain);
}

quint8 ColorSensor::readFromRegister(quint8 reg)
{
    return mProto->readRegister(CMD_REPEATED | reg); /* CMD: select desired register */
}

bool ColorSensor::writeToRegister(quint8 reg, quint8 value)
{
    return mProto->writeRegister(CMD_REPEATED | reg, value); /* CMD: select desired register */
}

void ColorSensor::initSensor()
{
    enableSensor();
    currentIntegrationTime = DEFAULT_INTEGRATION_TIME;
    currentRgbGain = DEFAULT_RGB_GAIN;
    setIntegrationTime(currentIntegrationTime);
    setRgbcGain(currentRgbGain);

/*
    qDebug() << "Enable: " << readFromRegister(ENABLE_REG);
    qDebug() << "Int. time: " << readFromRegister(ATIME_REG);
    qDebug() << "Ctrl: " << readFromRegister(CONTROL_REG);
*/
}

