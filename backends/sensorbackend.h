#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "sensors/sensor.h"

class SensorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY updateReading)
    Q_PROPERTY(QString sensorType READ getSensorType WRITE setSensorType)

public:
    explicit SensorBackend(QObject *parent = nullptr);
    ~SensorBackend();
    void setSensorType(QString sensorType);
    QString getSensorType() const;
    QString getReading();

signals:
    void updateReading();

protected:
    QString mSensorType;
    QTimer* mUpdateTimer {nullptr};
    QScopedPointer<Sensor> mSensor;
};

#endif // SENSORBACKEND_H
