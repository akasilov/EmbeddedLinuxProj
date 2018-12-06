import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.sensorbackend 1.0

Page {
    width: 800
    height: 480

    SensorBackend {
        sensorType: "color"
        id: colorSensor;
    }

    header: Label {
        text: qsTr("Color")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    Rectangle
    {
        x: 284
        y: -46
        width: 500
        height: 40
        color: "white"
        border.color: "black"
        border.width: 2
        radius: 5
        Text {
            x: 5
            y: 10
            text: colorSensor.sensorReading
            color: "grey"
        }
    }

}
