import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    width: 760
    height: 400

    header: Label {
        text: qsTr("Color")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
}