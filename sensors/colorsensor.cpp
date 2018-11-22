#include "colorsensor.h"
#include "drivers/i2cprotocol.h"
#include <QDebug>

ColorSensor::ColorSensor()
    : Sensor("ColorSensor", new I2CProtocol(4, ColorSensor::I2C_ADDRESS))
{}

QString ColorSensor::getReadingAsString()
{
    auto colors = getColors();
    return QString::number(colors.Clear) + " Red: " + QString::number(colors.Red) + " Green: " + QString::number(colors.Green) + " Blue: " + QString::number(colors.Blue);
}

/* read out color data from sensor */
rgbColorAdc ColorSensor::getColors()
{
    rgbColorAdc data = {0, 0, 0, 0};
    const quint8 length = 8;
    quint8 buffer[length];
    bool isOk = false;

    /* read all 8 color registers at once (Clear, Red, Green, Blue; high and low byte) */
    //if (data != Q_NULLPTR)
    {
        /* set adress to first CRGB register, use increment mode (inc. addr. every reg. read) */
        isOk = writeToRegister(CDATAL_REG, CMD_INCREMENT | CDATAL_REG);
        if (isOk == true)
        {
            isOk = mProto->readBytes(buffer, length);
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
        }
        if (isOk == true)
        {
            qDebug() << "clear: " << data.Clear << " red: " << data.Red << " green: " << data.Green << " blue: " << data.Blue;
        }
        else
        {
            qWarning() << "could not read color sensor data in file " << __FILE__ << " function " << __FUNCTION__ << " line " << __LINE__;

        }
    }
    //else
    {
        //qDebug() << "data has null pointer";
    }
    return data;
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
    writeToRegister(ATIME_REG, value);
}

void ColorSensor::setRgbcGain(quint8 value)
{
    writeToRegister(CONTROL_REG, value);
}

quint8 ColorSensor::readFromRegister(quint8 reg)
{
    mProto->writeByte(CMD_REPEATED | reg); /* CMD: select desired register */
    return mProto->readByte(); /* read from register */
}

bool ColorSensor::writeToRegister(quint8 reg, quint8 value)
{
    bool b = mProto->writeByte(CMD_REPEATED | reg); /* CMD: desired register */
    if (b == true)
    {
        b = mProto->writeByte(value); /* write to selected register */
    }
    return b;
}

void ColorSensor::initSensor()
{
    enableSensor();
    setIntegrationTime(0x01);
    setRgbcGain(0x02);

/*
    qDebug() << "Enable: " << readFromRegister(ENABLE_REG);
    qDebug() << "Int. time: " << readFromRegister(ATIME_REG);
    qDebug() << "Ctrl: " << readFromRegister(CONTROL_REG);
*/
}

