import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    width: 800
    height: 480

    header: Label {
        text: qsTr("Acceleration")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
}
