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
                swipeView.currentIndex -= 1;
            }
        }
        onButtonPressedT2:
        {
            /* swipe to right */
            if (swipeView.currentIndex < 2)
            {
                swipeView.currentIndex += 1;
            }
        }
        onButtonPressedT3:
        {
            /* center bubble */
            acceleration.acc_bubble.x = 350
            acceleration.acc_bubble.y = 190

        }
        onButtonPressedT4:
        {
            /* toggle logging on button pressed */
            if (logger_checkBox.checked)
            {
                /* switch off */
                logger_checkBox.checked = false;
                gpio.setLedL4(false);
            }
            else
            {
                /* switch on */
                logger_checkBox.checked = true;
                gpio.setLedL4(true);
            }

        }
    }

    SwipeView {
        id: swipeView
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

}
