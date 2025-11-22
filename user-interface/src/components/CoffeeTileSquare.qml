import QtQuick 2.15
import "text"
import "../theme"

Card {
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
            source: imageSource
            width: 90
            height: 90
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            TextDefault {
                text: name
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TextDefault {
                text: price
                color: Theme.white
                opacity: 0.7
                font.pixelSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
