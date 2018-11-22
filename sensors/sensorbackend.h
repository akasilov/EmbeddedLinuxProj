#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "sensor.h"

class SensorBackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY readingsChanged)
    Q_PROPERTY(QString sensorType READ getSensorType WRITE setSensorType)

public:
    explicit SensorBackEnd(QObject *parent = nullptr);
    void setSensorType(QString sensorType);
    QString getSensorType() const;
    QString getReading();

signals:
    void readingsChanged();

private:
    QString mSensorType;
    QTimer* updateTimer {nullptr};
    QScopedPointer<Sensor> mSensor;
};

#endif // SENSORBACKEND_H
