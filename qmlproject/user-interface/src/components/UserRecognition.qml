import QtQuick
import "../components/text"
import "../components"
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
            // Remove anchors.centerIn and anchors.verticalCenter
            // Row handles horizontal positioning automatically
            // For vertical alignment within the Row, use this:
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10

            H1 {
                // Remove anchors.centerIn - Column handles positioning
                text: "Hold on,"
            }
            
            TextDefault {
                text: "I'm trying to recognize you..."
            }
        }
    }
}