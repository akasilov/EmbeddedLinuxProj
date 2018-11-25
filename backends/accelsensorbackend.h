#ifndef ACCELSENSORBACKEND_H
#define ACCELSENSORBACKEND_H

#include "sensorbackend.h"

class AccelSensorBackend : public SensorBackend
{
    Q_OBJECT
    Q_PROPERTY(float x READ getX)
    Q_PROPERTY(float y READ getY)
    Q_PROPERTY(float z READ getZ)


public:
    explicit AccelSensorBackend(QObject *parent = nullptr);

    float getX();
    float getY();
    float getZ();
};

#endif // ACCELSENSORBACKEND_H
