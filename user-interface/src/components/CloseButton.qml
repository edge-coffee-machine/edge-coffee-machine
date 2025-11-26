import QtQuick
import "../theme"

Rectangle {
    id: root

    signal goBackRequested

    color: Theme.white
    height: 40
    radius: 20
    width: 40

    anchors {
        right: parent.right
        top: parent.top
    }

    // Rectangular Bar 1 (Rotated 45 degrees)
    Rectangle {
        anchors.centerIn: parent
        color: Theme.dark700 // Color of the 'X' lines
        height: 2.5 // Thickness of the bar
        radius: 2
        rotation: 45
        width: 22 // Length of the bar
    }

    // Rectangular Bar 2 (Rotated -45 degrees)
    Rectangle {
        anchors.centerIn: parent
        color: Theme.dark700 // Color of the 'X' lines
        height: 2.5 // Thickness should match Bar 1
        radius: 2
        rotation: -45
        width: 22 // Length should match Bar 1
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.goBackRequested()
    }
}
