import QtQuick
import "../"
import "../text"
import "../buttons"
import Theme

Rectangle {
    id: root

    // This property will receive the selected beverage object from main.qml
    property Beverage targetBeverage: null

    function getIngredientInfo(label: string, targetBeverage: Beverage): IngredientInfo {
        if (label === "Coffee") {
            return targetBeverage.coffeeBeans;
        }

        if (label === "Cocoa") {
            return targetBeverage.cocoaPowder;
        }

        if (label === "Water") {
            return targetBeverage.water;
        }

        if (label === "Foam") {
            return targetBeverage.foam;
        }

        return targetBeverage.milk;
    }

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

        Item {
            height: 30
            width: parent.width

            TextDefault {
                color: Theme.white
                font.bold: true
                font.pixelSize: 24
                text: "Settings"
            }

            Image {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                height: 18
                source: "assets/icons/reset.png"
                width: 18

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        console.log("Reset settings to default for " + root.targetBeverage.name);
                        root.targetBeverage.resetIngredients();
                    }
                }
            }
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
                    initialValue: getIngredientInfo(modelData, root.targetBeverage).current
                    label: modelData
                    maximumValue: getIngredientInfo(modelData, root.targetBeverage).max
                    minimumValue: getIngredientInfo(modelData, root.targetBeverage).min
                    unitOfM: "ml"
                    visible: getIngredientInfo(modelData, root.targetBeverage).min > 0 && getIngredientInfo(modelData, root.targetBeverage).def > 0
                    width: settingsColumn.width

                    onValueChanged: {
                        getIngredientInfo(modelData, root.targetBeverage).setCurrent(value);
                    }
                }
            }
        }
    }

    // Bottom card (fixed at bottom)
    CardWithLoading {
        id: bottomCard

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        brewingTime: root.targetBeverage ? root.targetBeverage.brewingTime() : 0
        color: Theme.dark400
        height: 130
        targetBeverage: root.targetBeverage
    }
}
