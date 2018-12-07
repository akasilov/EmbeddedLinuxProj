import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.sensorbackend 1.0

Page {
    width: 800
    height: 480

    Rectangle {
        id: rectangle
        x: 234
        y: 84
        width: 333
        height: 236
        color: "#822b2b"
    }

    SensorBackend {
        sensorType: "color"
        id: colorSensor
        onReadingsUpdated: {
            var sensorData = colorSensor.sensorData
            var red = sensorData[1]/32767.
            var green = sensorData[2]/32767.
            var blue = sensorData[3]/32767.
            rectangle.color = Qt.rgba(red, green, blue, 1)
        }
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
