import QtQuick
import Theme
import "text"

Rectangle {
    id: header

    // Signal emitted when "not you?" button is clicked
    signal switchUserRequested

    color: "transparent"

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        H1 {
            anchors.verticalCenter: parent.verticalCenter
            text: "Good Morning,"
        }

        H1 {
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 32
            font.weight: 300
            text: EdgeCoffeeMachine.user ? (EdgeCoffeeMachine.user.name + " " + EdgeCoffeeMachine.user.surname ): "Guest"
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
                text: EdgeCoffeeMachine.user ? "Not you?" : "Remember me"
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("switch user");
                    header.switchUserRequested();
                }
            }
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            visible: EdgeCoffeeMachine.isRecognizingUser

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                height: 20
                opacity: 0.2
                width: 1
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                height: 10
                width: 10
                radius: 5
                opacity: 0.8
            }

            TextDefault {
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 14
                font.weight: 300
                text: "Recognizing..."
            }
        }
    }

    Image {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: 24
        source: "assets/icons/logout.png"
        visible: EdgeCoffeeMachine.user
        width: 24

        MouseArea {
            anchors.fill: parent

            onClicked: {
                console.log("Logout clicked");
                EdgeCoffeeMachine.logoutUser();
            }
        }
    }
}
