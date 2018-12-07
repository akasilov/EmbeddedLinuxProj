#ifndef SENSOR_H
#define SENSOR_H

#include <QScopedPointer>
#include <QScopedPointer>
#include <QVariantList>
#include "drivers/i2cprotocol.h"

class Sensor
{
public:
    Sensor(QString name, I2CProtocol* proto);
    virtual ~Sensor();

    virtual QString getReadingAsString() = 0;
    virtual void readSensorData() = 0;
    virtual QVariantList getSensorData() = 0;

protected:
    QString mSensorName;
    QScopedPointer<I2CProtocol> mProto;
};

#endif // SENSOR_H
