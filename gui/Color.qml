import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.sensorbackend 1.0
import QtGraphicalEffects 1.0

Page {
    id: colorPageId
    width: 800
    height: 480
    property alias colorPage_bubbleColor: colorSensor.bubbleColor
    //property var colorPage_onColorChanged: onColorChanged
    //signal onColorChanged(variant rgba_color)

    SensorBackend {
        sensorType: "color"
        id: colorSensor
        property color bubbleColor: "red"

        onReadingsUpdated: {
            var data = colorSensor.sensorData
            var red = data[1]
            var green = data[2]
            var blue = data[3]
            var maxc = 0;

            /* color sensitivity compensation (-> datasheet) */
            red *= 0.9
            green *= 1.33
            blue *= 1.57

            /* get max color value */
            if (red > green)
                maxc = red
            else
                maxc = green
            if (maxc < blue)
                maxc = blue

            /* normalize */
            red = red / maxc
            green = green / maxc
            blue = blue / maxc

            /* print values to display  */
            tinttext.text = data[4]
            rrtext.text = red.toFixed(3)
            rgtext.text = green.toFixed(3)
            rbtext.text = blue.toFixed(3)

            /* set color */
            bubbleColor = Qt.rgba(red, green, blue, 1)
            color_circle.color = bubbleColor

            /* emit onColorChanged signal */
            //onColorChanged(bubbleColor)
        }
    }

    header: Label {
        text: qsTr("Color")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    Rectangle
    {
        id: color_circle
        x: 239
        y: 86
        width: 310
        height: 178
        color: Qt.hsla(0, 0.5, 0.5, 1)
        radius: 5
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

    TextArea {
        id: textArea_red
        x: 56
        y: 165
        text: qsTr("red")
    }

    Rectangle {
        id: rectangle_red
        x: 125
        y: 165
        width: 70
        height: 29
        color: "white"
        radius: 0
        border.width: 1
        Text
        {
            id: rrtext
            x: 10
            y: 5
            color: "grey"
        }
    }

    TextArea {
        id: textArea_green
        x: 56
        y: 200
        text: qsTr("green")
    }

    Rectangle {
        id: rectangle_green
        x: 125
        y: 200
        width: 70
        height: 29
        color: "white"
        radius: 0
        border.width: 1
        Text
        {
            id: rgtext
            x: 10
            y: 5
            color: "grey"
        }
    }

    TextArea {
        id: textArea_blue
        x: 56
        y: 235
        text: qsTr("blue")
    }

    Rectangle {
        id: rectangle_blue
        x: 125
        y: 235
        width: 70
        height: 29
        color: "white"
        radius: 0
        border.width: 1
        Text
        {
            id: rbtext
            x: 10
            y: 5
            color: "grey"
        }
    }

    TextArea {
        id: textArea_integrationTime
        x: 56
        y: 86
        text: qsTr("ATIME")
    }

    Rectangle {
        id: rectangle_integrationTime
        x: 125
        y: 86
        width: 70
        height: 29
        color: "#ffffff"
        radius: 0
        Text {
            id: tinttext
            x: 10
            y: 5
            color: "#808080"
        }
        border.width: 1
    }

    TextArea {
        id: textArea_red1
        x: 56
        y: 130
        text: qsTr("normalized:")
    }

}
