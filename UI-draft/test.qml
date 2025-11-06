import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    property double spacing: 10

    Rectangle {
        width: 1000; height: 40; color: "transparent"

        Text {
            id: title
            text: "Good Morning, "
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            x: 10
            font.bold: true
            font.pixelSize: 30
        }

        Text {
            id: userName
            text: "Matteo"
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: title.right
            font.pixelSize: 30
        }

        LightButton {
            id: myButton
            anchors.centerIn: parent
            text: "Remember Me"
            textColor: "#D9A569"
            onClick: console.log("Button clicked!")
            anchors.left: userName.right
        }
    }
}
