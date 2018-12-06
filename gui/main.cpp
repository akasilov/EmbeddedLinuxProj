#include <QApplication>
#include <QQmlApplicationEngine>
#include "backends/sensorbackend.h"
#include "backends/gpiobackend.h"
#include "drivers/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    qmlRegisterType<SensorBackend>("backends.sensorbackend", 1, 0, "SensorBackend");
    qmlRegisterType<GpioBackend>("backends.gpiobackend", 1, 0, "GpioBackend");
    qmlRegisterType<Logger>("seriallogger", 1, 0, "Logger");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
