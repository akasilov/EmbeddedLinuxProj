#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "sensors/i2cdevice.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    I2CDevice s("Accelerometer", 4, 0x4A);
    s.openDevice();


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
