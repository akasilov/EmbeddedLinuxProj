QT += quick svg
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gui/main.cpp \
    sensors/lightsensor.cpp \
    sensors/accelsensor.cpp \
    sensors/colorsensor.cpp \
    sensors/sensor.cpp \
    drivers/i2cprotocol.cpp \
    drivers/gpiobase.cpp \
    drivers/gpiobutton.cpp \
    drivers/gpioled.cpp \
    backends/sensorbackend.cpp \
    backends/gpiobackend.cpp \
    drivers/logger.cpp

RESOURCES += gui/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
target.path = /home/student/qt5
INSTALLS += target

HEADERS += \
    sensors/lightsensor.h \
    sensors/accelsensor.h \
    sensors/colorsensor.h \
    sensors/sensor.h \
    drivers/i2cprotocol.h \
    drivers/gpiobase.h \
    drivers/gpiobutton.h \
    drivers/gpioled.h \
    backends/sensorbackend.h \
    backends/gpiobackend.h \
    drivers/logger.h
