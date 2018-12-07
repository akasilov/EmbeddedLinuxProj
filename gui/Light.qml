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
                //format: "hh mm ss.zzz"
                tickCount:5
                min: 0
                max: 5

            }
            axisY: ValueAxis {
                min: 0
                max: 100
            }
        }
    }

    SensorBackend {
        sensorType: "light"
        id: lightSensor
        property int i: 0
        property int amountOfData: 0
        onReadingsUpdated: {
            var lightInLux = lightSensor.sensorData
            var currentDate = new Date

            if(amountOfData > series1.axisX.max){
                series1.axisX.min++;
                series1.axisX.max++;
            } else {
                amountOfData++; //This else is just to stop incrementing the variable unnecessarily
            }

            series1.append(i, lightInLux)
            i=i+1
        }
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
