#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include "sensors/sensor.h"

class SensorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY readingsUpdated)
    Q_PROPERTY(QString sensorType READ getSensorType WRITE setSensorType)
    Q_PROPERTY(QVariantList sensorData READ getSensorData NOTIFY readingsUpdated)

public:
    explicit SensorBackend(QObject *parent = nullptr);
    ~SensorBackend();
    void setSensorType(QString sensorType);
    QString getSensorType() const;
    QString getReading();
    QVariantList getSensorData();

public slots:
    virtual void updateReading();
signals:
    void readingsUpdated();

protected:
    QString mSensorType;
    QTimer* mUpdateTimer {nullptr};
    QScopedPointer<Sensor> mSensor;
};

#endif // SENSORBACKEND_H
