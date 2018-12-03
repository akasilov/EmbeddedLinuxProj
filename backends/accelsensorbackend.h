#ifndef ACCELSENSORBACKEND_H
#define ACCELSENSORBACKEND_H

#include "sensorbackend.h"

class AccelSensorBackend : public SensorBackend
{
    Q_OBJECT
    Q_PROPERTY(float x READ getX NOTIFY readingsUpdated)
    Q_PROPERTY(float y READ getY NOTIFY readingsUpdated)
    Q_PROPERTY(float z READ getZ NOTIFY readingsUpdated)


public:
    explicit AccelSensorBackend(QObject *parent = nullptr);

    float getX();
    float getY();
    float getZ();

    float x;
    float y;
    float z;

public slots:
    virtual void updateReading();

};

#endif // ACCELSENSORBACKEND_H
