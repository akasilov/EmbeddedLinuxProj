#ifndef SENSOR_H
#define SENSOR_H

#include <QScopedPointer>
#include <QScopedPointer>
#include <QVector>
#include "drivers/i2cprotocol.h"

class Sensor
{
public:
    Sensor(QString name, I2CProtocol* proto);
    virtual ~Sensor();

    virtual QString getReadingAsString() = 0;
    virtual void readSensorData() = 0;

    virtual QVector<float> readDataAsFloat() { return QVector<float>();}
    virtual QVector<qint32> readDataAsInt() { return QVector<qint32>();}

protected:
    QString mSensorName;
    QScopedPointer<I2CProtocol> mProto;
};

#endif // SENSOR_H
