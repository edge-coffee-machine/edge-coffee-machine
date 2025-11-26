import QtQuick
import "text"
import "../theme"

Card {
    id: root

    // Props
    property string imageSource: "../../assets/img/cappuccino.png"
    property string name: "Cappuccino"
    property string price: "€1.20"

    color: Theme.dark400
    height: 170
    width: 170

    Column {
        height: parent.height
        spacing: 10
        width: parent.width

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            height: 90
            source: root.imageSource
            width: 90
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            TextDefault {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
                text: root.name
            }

            TextDefault {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.white
                font.pixelSize: 15
                opacity: 0.7
                text: root.price
            }
        }
    }
}
