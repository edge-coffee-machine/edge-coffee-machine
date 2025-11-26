import QtQuick
import "text"
import "../theme"

Card {
    id: root

    // Props
    property string imageSource: "../../assets/img/cappuccino.png"
    property string price: "€1.20"
    property string name: "Cappuccino"

    color: Theme.dark400
    height: 170
    width: 170

    Column {
        spacing: 10
        width: parent.width
        height: parent.height

        Image {
            source: root.imageSource
            width: 90
            height: 90
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            TextDefault {
                text: root.name
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TextDefault {
                text: root.price
                color: Theme.white
                opacity: 0.7
                font.pixelSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
