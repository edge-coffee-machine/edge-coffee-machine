import QtQuick 2.15
import "../theme"
import "text"

Row {
    id: header
    spacing: 10

    // Signal emitted when "not you?" button is clicked
    signal switchUserRequested()

    H1 {
        text: "Good Morning,"
        anchors.verticalCenter: parent.verticalCenter
    }

    H1 {
        text: "Matteo"
        font.weight: 400
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        height: 20
        width: 1
        opacity: 0.2
    }

    Rectangle {
            id: notYouBtn
            width: 80; height: 28
            radius: 14
            color: Theme.dark400
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: "Not you?"
                color: Theme.accent
                anchors.centerIn: parent
                font.pixelSize: 14
                font.weight: 300
            }

            MouseArea {
                anchors.fill: parent
                onClicked: header.switchUserRequested()
            }
        }

}
