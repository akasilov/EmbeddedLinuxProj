#include "sensorbackend.h"

SensorBackEnd::SensorBackEnd(QObject *parent) :
    QObject(parent),
    mLightValueLux(0.)
{
    updateReading();
}

QString SensorBackEnd::getReading()
{
    return QString::number(mLightValueLux);
}

void SensorBackEnd::updateReading() {
    mLightValueLux = mSensor.getLightInLux();
    emit readingsChanged();
}
