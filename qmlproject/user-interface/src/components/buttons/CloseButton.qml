import QtQuick 2.0
import Theme

Rectangle {
    id: root

    signal goBackRequested

    color: Theme.white
    height: 40
    radius: 20
    width: 40

    // Rectangular Bar 1 (Rotated 45 degrees)
    Image {
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        source: "assets/icons/close-icon.png"
        width: 16
        height: 16
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.goBackRequested()
    }
}
