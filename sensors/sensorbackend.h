#ifndef SENSORBACKEND_H
#define SENSORBACKEND_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "lightsensor.h"
#include "colorsensor.h"

class SensorBackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sensorReading READ getReading NOTIFY readingsChanged)
    Q_PROPERTY(QString cSensorReading READ getColorReading NOTIFY readingsChanged)

public:
    explicit SensorBackEnd(QObject *parent = nullptr);
    QString getReading();
    QString getColorReading();

public slots:
    void updateReading();

signals:
    void readingsChanged();

private:
    QTimer *updateTimer;
    LightSensor mSensor;
    double mLightValueLux;
    ColorSensor cSensor;
    rgbColorAdc cData;
};

#endif // SENSORBACKEND_H
