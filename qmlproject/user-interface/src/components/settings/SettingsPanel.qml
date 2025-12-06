import QtQuick
import "../"
import "../text"
import "../buttons"
import Theme

Rectangle {
    id: root

    // This property will receive the selected beverage object from main.qml
    property Beverage targetBeverage: null

    color: Theme.dark500
    height: 390
    radius: Theme.cardRadius
    width: 275

    // Header (fixed at top)
    Column {
        id: header

        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 15
        spacing: 5

        TextDefault {
            color: Theme.white
            font.bold: true
            font.pixelSize: 24
            text: "Settings"
        }

        Subtitle {
            text: "Customize your drink."
        }
    }

    // Scrollable settings area
    Flickable {
        id: flickable

        anchors.bottom: bottomCard.top
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: header.bottom
        anchors.topMargin: 10
        clip: true
        //height: parent.height - header.height - bottomCard.height - 60
        contentHeight: 200 // Note, the previous code does not seem to work: settingsColumn.height
        contentWidth: width

        Column {
            id: settingsColumn

            spacing: 0
            width: parent.width

            

            Repeater {
                id: sliderGroup

                model: ["Coffee", "Cocoa", "Water", "Foam", "Milk"]

                delegate: SliderInput {
                    width: settingsColumn.width
                    label: modelData

                    maximumValue: 50
                    unitOfM: "ml"
                }
            }
        }
    }

    // Bottom card (fixed at bottom)
    CardWithLoading {
        id: bottomCard

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        anchors.bottomMargin: 15
        targetBeverage: root.targetBeverage
        brewingTime: root.targetBeverage ? root.targetBeverage.brewingTime() : 0
        color: Theme.dark400
        height: 130
    }
}