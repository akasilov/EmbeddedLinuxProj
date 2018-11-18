#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include "lightsensor.h"

class SensorBackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY readingsChanged)

public:
    explicit SensorBackEnd(QObject *parent = nullptr);
    QString getReading();
public slots:
    void updateReading();

signals:
    void readingsChanged();

private:
    LightSensor mSensor;
    double mLightValueLux;
};

#endif // SENSORBACKEND_H
