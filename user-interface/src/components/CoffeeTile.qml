import QtQuick 2.15
import "text"
import "../theme"

Card {
    // Props
    property string imageSource: "../../assets/img/cappuccino.png"
    property string price: "€1.20"
    property string name: "Cappuccino"

    color: Theme.dark400
    height: 80
    width: 235

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 20

        Image {
            source: imageSource
            width: 60
            height: 60
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5

            TextDefault {
                text: name
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
            }

            TextDefault {
                text: price
                color: Theme.white
                opacity: 0.7
                font.pixelSize: 15
            }
        }
    }
}
