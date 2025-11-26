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
    height: 80
    width: 235

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 20

        Image {
            anchors.verticalCenter: parent.verticalCenter
            height: 60
            source: root.imageSource
            width: 60
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5

            TextDefault {
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
                text: root.name
            }

            TextDefault {
                color: Theme.white
                font.pixelSize: 15
                opacity: 0.7
                text: root.price
            }
        }
    }
}
