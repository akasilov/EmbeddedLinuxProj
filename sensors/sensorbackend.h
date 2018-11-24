#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "sensor.h"
#include "./drivers/gpiobutton.h"
#include "./drivers/gpioled.h"

class SensorBackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY readingsChanged)
    Q_PROPERTY(QString sensorType READ getSensorType WRITE setSensorType)

public:
    explicit SensorBackEnd(QObject *parent = nullptr);
    ~SensorBackEnd();
    void setSensorType(QString sensorType);
    QString getSensorType() const;
    QString getReading();

    gpioButton T1;
    gpioButton T2;
    //gpioButton T3;
    //gpioButton T4;

public slots:
    void buttonT1_event();
    void buttonT2_event();
    void updateReading();

signals:
    void readingsChanged();

private:
    QString mSensorType;
    QTimer* updateTimer {nullptr};
    QScopedPointer<Sensor> mSensor;
};

#endif // SENSORBACKEND_H
