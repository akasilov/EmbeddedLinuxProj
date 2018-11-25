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

    virtual float getX() {return 0.;}
    virtual float getY() {return 0.;}
    virtual float getZ() {return 0.;}


protected:
    quint8 readRegister(quint8 reg);
    bool writeRegister(quint8 reg, quint8 value);

    QString mSensorName;
    QScopedPointer<I2CProtocol> mProto;
};

#endif // SENSOR_H
