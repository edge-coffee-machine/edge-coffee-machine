import QtQuick 2.15
import "../theme"
import "text"

Row {
    id: header
    spacing: 10

    // Signal emitted when "not you?" button is clicked
    signal switchUserRequested

    H1 {
        text: "Good Morning,"
        anchors.verticalCenter: parent.verticalCenter
    }

    H1 {
        text: edgeCoffeeMachineController.user ? edgeCoffeeMachineController.user.name : "Guest"
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
        width: btnText.width + 30
        height: 28
        radius: 14
        color: Theme.dark400
        anchors.verticalCenter: parent.verticalCenter

        TextDefault {
            id: btnText
            text: edgeCoffeeMachineController.user ? "Not you?" : "Remember me"
            color: Theme.accent
            anchors.centerIn: parent
            font.pixelSize: 14
            font.weight: 400
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("switch user")
                header.switchUserRequested()
            }
        }
    }
}
