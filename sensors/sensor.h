#ifndef SENSOR_H
#define SENSOR_H

#include <QScopedPointer>
#include <QString>
#include "drivers/i2cprotocol.h"

class Sensor
{
public:
    Sensor(QString name, I2CProtocol* proto);
    virtual QString getReadingAsString() = 0;
    virtual ~Sensor();

protected:
    QString mSensorName;
    QScopedPointer<I2CProtocol> mProto;
};

#endif // SENSOR_H
