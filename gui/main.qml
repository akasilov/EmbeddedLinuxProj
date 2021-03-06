import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.gpiobackend 1.0
import seriallogger 1.0

ApplicationWindow {
    id:root
    visible: true
    width: 800
    height: 480
    title: qsTr("Tabs")

    GpioBackend {
        id: gpio
        onButtonPressedT1:
        {
            /* swipe to left */
            if (swipeView.currentIndex > 0)
            {
                swipeView.currentIndex -= 1
            }
        }
        onButtonPressedT2:
        {
            /* swipe to right */
            if (swipeView.currentIndex < 2)
            {
                swipeView.currentIndex += 1
            }
        }
        onButtonPressedT3:
        {
            /* toggle logging on button pressed */
            if (logger_checkBox.checked)
            {
                /* switch off */
                logger_checkBox.checked = false
                gpio.setLedL4(false)
            }
            else
            {
                /* switch on */
                logger_checkBox.checked = true
                gpio.setLedL4(true)
            }
        }
        onButtonPressedT4:
        {
            /* quit application */
            //applicationClosed_rectangle.visible = true
            Qt.quit()
        }

        onUpdateTimerExpired:
        {
            /* set bubble color depending on current color sensor measurement */
            acceleration.acc_bubble.color = colorpage.colorPage_bubbleColor
        }
    }

    SwipeView {
        id: swipeView
        visible: true
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Acceleration {
            id: acceleration
        }

        Light {
            id: light
        }

        Color {
            id: colorpage
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Acceleration")
        }

        TabButton {
            text: qsTr("Light")
        }

        TabButton {
            text: qsTr("Color")
        }
    }

    Logger {
        id: logger
    }

    CheckBox {
        id: logger_checkBox
        x: -140
        y: 55
        text: qsTr("log acceleration data")
        visible: false
    }

    /*
    Rectangle {
        id: applicationClosed_rectangle
        x: 44
        y: 133
        width: 718
        height: 53
        color: "#444444"
        radius: 10
        z: 2
        anchors.horizontalCenter: parent.horizontalCenter
        border.color: "#646464"
        visible: false
        Text
        {
            color: "#d2bfbf"
            text: "Application closed"
            visible: parent.visible
            font.pointSize: 26
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
        }
    }
*/
}
