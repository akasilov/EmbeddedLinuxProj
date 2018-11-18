#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "sensors/lightsensor.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    LightSensor s;
    auto lux = s.getLightValueInLux();
    s.closeDevice();


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
