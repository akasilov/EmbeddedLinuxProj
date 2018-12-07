import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.sensorbackend 1.0
import QtCharts 2.1

Page {
    width: 800
    height: 480

    ChartView {
        anchors.fill: parent
        antialiasing: true
        id: chartView
        SplineSeries {
            id: series1
            name: "Light in lux"

            axisX: ValueAxis {
                tickCount: 5

            }
            axisY: ValueAxis {
                min: 0
                max: 1
            }
        }
    }

    SensorBackend {
        sensorType: "light"
        id: lightSensor
        property date startTime: new Date()
        onReadingsUpdated: {
            var lightInLux = lightSensor.sensorData[0]

            var msecs = toMsecsSinceEpoch(new Date()) - toMsecsSinceEpoch(startTime)
            msecs = msecs/1000
            series1.axisX.max = msecs + 5
            if (lightInLux > series1.axisY.max) {
                series1.axisY.max = lightInLux + 10
            }
            series1.append(msecs, lightInLux)
        }
    }

    function toMsecsSinceEpoch(date) {
        var msecs = date.getTime();
        return msecs;
    }

    header: Label {
        text: qsTr("Light")
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
            text: lightSensor.sensorReading
            color: "grey"
        }
    }
}
