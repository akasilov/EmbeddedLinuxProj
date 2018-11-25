import QtQuick 2.9
import QtQuick.Controls 2.2
import backends.gpiobackend 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 480
    title: qsTr("Tabs")

    GpioBackend {
        id: gpio
        onButtonPressedT1: swipeView.currentIndex = 0
        onButtonPressedT2: swipeView.currentIndex = 1
        onButtonPressedT3: swipeView.currentIndex = 2
    }
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Light {
        }

        Acceleration {
        }

        Color {
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Light")
        }

        TabButton {
            text: qsTr("Acceleration")
        }
        TabButton {
            text: qsTr("Color")
        }
    }
}
