import QtQuick 2.11
import QtQuick.Controls 2.4

Page {
    width: 800
    height: 480

    header: Label {
        text: qsTr("Color")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
}
