import QtQuick
import QtQuick.Layouts
import "../"
import "../text"
import "../buttons"
import "../../theme"

Card {
    id: root

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

    color: Theme.dark500
    height: 390
    width: 275

    // Header (fixed at top)
    Column {
        id: header

        anchors.left: parent.left
        anchors.margins: 10
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 5
        visible: root.targetBeverage !== null

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
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: header.bottom
        anchors.topMargin: 10
        clip: true
        contentHeight: settingsColumn.height
        contentWidth: width
        visible: root.targetBeverage !== null

        Column {
            id: settingsColumn

            spacing: 0
            width: parent.width

            Repeater {
                id: sliderGroup

                model: ["Coffee", "Cocoa", "Water", "Foam", "Milk"]

                delegate: SliderInput {
                    label: modelData
                    maximumValue: {
                        if (!targetBeverage)
                            return 0;
                        switch (modelData) {
                        case "Coffee":
                            return targetBeverage.maxCoffeeBeans;
                        case "Cocoa":
                            return targetBeverage.maxCocoaPowder;
                        case "Water":
                            return targetBeverage.maxWater;
                        case "Foam":
                            return targetBeverage.maxFoam;
                        case "Milk":
                            return targetBeverage.maxMilk;
                        default:
                            return 0;
                        }
                    }
                    unitOfM: {
                        if (modelData == "Cocoa")
                            return "g";
                        else
                            return "ml";
                    }

                    // Bind value based on the ingredient name
                    value: {
                        if (!targetBeverage)
                            return 0;
                        switch (modelData) {
                        case "Coffee":
                            return targetBeverage.coffeeBeans;
                        case "Cocoa":
                            return targetBeverage.cocoaPowder;
                        case "Water":
                            return targetBeverage.water;
                        case "Foam":
                            return targetBeverage.foam;
                        case "Milk":
                            return targetBeverage.milk;
                        default:
                            return 0;
                        }
                    }
                    width: settingsColumn.width

                    // Update the beverage property when slider changes
                    onValueChanged: {
                        if (!targetBeverage)
                            return;
                        switch (modelData) {
                        case "Coffee":
                            targetBeverage.coffeeBeans = value;
                            break;
                        case "Cocoa":
                            targetBeverage.cocoaPowder = value;
                            break;
                        case "Water":
                            targetBeverage.water = value;
                            break;
                        case "Foam":
                            targetBeverage.foam = value;
                            break;
                        case "Milk":
                            targetBeverage.milk = value;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Bottom card (fixed at bottom)
    CardWithLoading {
        id: bottomCard

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 10
        anchors.right: parent.right
        brewingTime: root.targetBeverage ? root.targetBeverage.brewingTime() : 0
        color: Theme.dark400
        height: 130
        isLoading: !makeButton.enabled
        visible: root.targetBeverage !== null

        TextDefault {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 6
            color: Theme.white
            font.bold: true
            font.pixelSize: 40
            text: makeButton.enabled ? "€ 1.20" : "Brewing"
        }

        CustomButton {
            id: makeButton

            anchors.bottom: parent.bottom
            color: Theme.dark700
            enabled: !edgeCoffeeMachineController.isMakingDrink
            text: makeButton.enabled ? ("Make " + (targetBeverage ? targetBeverage.name : "")) : "Cancel"
            textWeight: 700

            onClick: {
                edgeCoffeeMachineController.makeDrink(root.targetBeverage);
            }
        }
    }

    // Placeholder for when no beverage is selected
    TextDefault {
        anchors.centerIn: parent
        color: "#808080"
        font.pixelSize: 18
        text: "Select a drink to see details"
        visible: root.targetBeverage === null
        wrapMode: Text.WordWrap
    }
}
