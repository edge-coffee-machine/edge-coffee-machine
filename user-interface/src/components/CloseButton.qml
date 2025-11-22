import QtQuick 2.15
import "../theme"

Rectangle {
    width: 40
    height: 40
    radius: 20
    color: Theme.white

    signal goBackRequested

    anchors {
        top: parent.top
        right: parent.right
    }

    // Rectangular Bar 1 (Rotated 45 degrees)
    Rectangle {
        width: 22 // Length of the bar
        height: 2.5 // Thickness of the bar
        color: Theme.dark700 // Color of the 'X' lines
        anchors.centerIn: parent
        rotation: 45
        radius: 2
    }

    // Rectangular Bar 2 (Rotated -45 degrees)
    Rectangle {
        width: 22 // Length should match Bar 1
        height: 2.5 // Thickness should match Bar 1
        color: Theme.dark700 // Color of the 'X' lines
        anchors.centerIn: parent
        rotation: -45
        radius: 2
    }

    MouseArea {
        anchors.fill: parent
        onClicked: goBackRequested()
    }
}
