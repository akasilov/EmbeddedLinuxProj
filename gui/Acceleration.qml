import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.sensorbackend 1.0

Page {
    id: page
    width: 800
    height: 480
    readonly property double radians_to_degrees: 180 / Math.PI
    property alias acc_bubble: bubble

    SensorBackend {
        sensorType: "acceleration"
        id: accelSensor
        onReadingsUpdated: {
            var sensorData = accelSensor.sensorData
            var ax = sensorData[0]
            var ay = sensorData[1]
            var az = sensorData[2]
            var newX = (bubble.x + calcRoll(ax, ay, az) * .1)
            var newY = (bubble.y - calcPitch(ax, ay, az) * .1)

            if (isNaN(newX) || isNaN(newY))
                return;

            if (newX < 0)
                newX = 0

            if (newX > page.width - bubble.width)
                newX = page.width - bubble.width

            if (newY < 18)
                newY = 18

            if (newY > page.height - bubble.height)
                newY = page.height - bubble.height

                bubble.x = newX
                bubble.y = newY

            if (logger_checkBox.checked == true)
            {
                logger.sendString("\n" + Date.now() + "\t" + ax.toPrecision(7) + "\t" + ay.toPrecision(7) + "\t" + az.toPrecision(7))
            }
        }
    }
    header: Label {
        text: qsTr("Acceleration")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    function calcPitch(x,y,z) {
        return -Math.atan2(y, Math.sqrt(x * x + z * z)) * page.radians_to_degrees;
    }
    function calcRoll(x,y,z) {
        return -Math.atan2(x, Math.sqrt(y * y + z * z)) * page.radians_to_degrees;
    }

    Image {
        id: bubble
        source: "content/Bluebubble.svg"
        smooth: true
        property real centerX: page.width / 2
        property real centerY: page.height / 2
        property real bubbleCenter: bubble.width / 2
        x: centerX - bubbleCenter
        y: centerY - bubbleCenter

        Behavior on y {
            SmoothedAnimation {
                easing.type: Easing.Linear
                duration: 100
            }
        }
        Behavior on x {
            SmoothedAnimation {
                easing.type: Easing.Linear
                duration: 100
            }
        }
    }

    Rectangle
    {
        x: 294
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
            text: accelSensor.sensorReading
            color: "grey"
        }
    }

}
