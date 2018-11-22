import QtQuick 2.9
import QtQuick.Controls 2.2
import sensors.sensorbackend 1.0

Page {
    width: 800
    height: 480

    header: Label {
        text: qsTr("Color")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    SensorBackEnd
    {
        id: colorSensor;

    }

    Rectangle
    {
        x: 150
        y: 100
        width: 500
        height: 40
        color: "white"
        border.color: "black"
        border.width: 2
        radius: 5
        Text {
            x: 5
            y: 10
            text: colorSensor.cSensorReading
            color: "grey"
        }
    }
}
