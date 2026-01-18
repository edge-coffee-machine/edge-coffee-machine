import QtQuick
import "../components" as Components
import Theme

Rectangle {
    id: root

    color: "transparent"
    height: 600
    width: 1024

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 40

        Rectangle {
            radius: 150
            width: 150
            height: 150
            color: "white"

            Image {
                anchors.centerIn: parent
                source: "assets/images/ai-wave.png"
                width: parent.width
                height: parent.height
                fillMode: Image.PreserveAspectFit
            }
        }

        Column {
            anchors.centerIn: parent
            anchors.verticalCenter: parent.verticalCenter

            H1 {
                text: "Hold on,"
                anchors.centerIn: parent
            }
            TextDefault {
                text: "I’m trying to recognize you..."
            }
        }
    }

    


}