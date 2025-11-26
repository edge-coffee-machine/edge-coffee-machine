import QtQuick
import "../theme"
import "text"

Row {
    id: header

    // Signal emitted when "not you?" button is clicked
    signal switchUserRequested

    spacing: 10

    H1 {
        anchors.verticalCenter: parent.verticalCenter
        text: "Good Morning,"
    }

    H1 {
        anchors.verticalCenter: parent.verticalCenter
        font.weight: 400
        text: edgeCoffeeMachineController.user ? edgeCoffeeMachineController.user.name : "Guest"
    }

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        height: 20
        opacity: 0.2
        width: 1
    }

    Rectangle {
        id: notYouBtn

        anchors.verticalCenter: parent.verticalCenter
        color: Theme.dark400
        height: 28
        radius: 14
        width: btnText.width + 30

        TextDefault {
            id: btnText

            anchors.centerIn: parent
            color: Theme.accent
            font.pixelSize: 14
            font.weight: 400
            text: edgeCoffeeMachineController.user ? "Not you?" : "Remember me"
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                console.log("switch user");
                header.switchUserRequested();
            }
        }
    }
}
