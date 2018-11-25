#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "backends/sensorbackend.h"
#include "backends/accelsensorbackend.h"
#include "backends/gpiobackend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<SensorBackend>("backends.sensorbackend", 1, 0, "SensorBackend");
    qmlRegisterType<AccelSensorBackend>("backends.accelsensorbackend", 1, 0, "AccelSensorBackend");
    qmlRegisterType<GpioBackend>("backends.gpiobackend", 1, 0, "GpioBackend");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
