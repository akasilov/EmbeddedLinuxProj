import QtQuick 2.9
import QtQuick.Controls 2.2
import sensors.sensorbackend 1.0

Page {
    width: 800
    height: 480

    header: Label {
        text: qsTr("Light")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    SensorBackEnd {
        id: lightSensor
    }

    TextArea {
        id: luxValue
        text: lightSensor.sensorReading
        anchors.verticalCenterOffset: -73
        anchors.horizontalCenterOffset: 10
        anchors.centerIn: parent
    }

    Button {
        id: button
        width: 200
        height: 40
        anchors.verticalCenterOffset: -27
        anchors.horizontalCenterOffset: 10
        anchors.centerIn: parent
        text: qsTr("Read")
    }

    Connections {
        target: button
        onClicked: lightSensor.updateReading()
    }
}
