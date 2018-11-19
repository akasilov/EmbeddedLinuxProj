import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: page
    width: 800
    height: 480

    header: Label {
        text: qsTr("Acceleration")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    Image {
        id: image
        x: 375
        y: 162
        width: 50
        height: 50
        source: "content/Bluebubble.svg"
    }
}
