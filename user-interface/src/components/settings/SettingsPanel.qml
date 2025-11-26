import QtQuick
import QtQuick.Layouts
import "../"
import "../text"
import "../buttons"
import "../../theme"

Card {
    id: root
    width: 275
    height: 390
    color: Theme.dark500

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

    // Header (fixed at top)
    Column {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 5
        visible: root.targetBeverage !== null

        TextDefault {
            text: "Settings"
            color: Theme.white
            font.pixelSize: 24
            font.bold: true
        }

        Subtitle {
            text: "Customize your drink."
        }
    }

    // Scrollable settings area
    Flickable {
        id: flickable
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bottomCard.top
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        visible: root.targetBeverage !== null
        clip: true

        contentHeight: settingsColumn.height
        contentWidth: width

        Column {
            id: settingsColumn
            width: parent.width
            spacing: 0

            Repeater {
                id: sliderGroup
                model: ["Coffee", "Cocoa", "Water", "Foam", "Milk"]
                delegate: SliderInput {
                    width: settingsColumn.width
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
        anchors.right: parent.right
        anchors.margins: 10
        height: 130
        color: Theme.dark400
        visible: root.targetBeverage !== null
        isLoading: !makeButton.enabled
        brewingTime: root.targetBeverage ? root.targetBeverage.brewingTime() : 0

        TextDefault {
            text: makeButton.enabled ? "€ 1.20" : "Brewing"
            font.pixelSize: 40
            font.bold: true
            color: Theme.white
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 6
        }

        CustomButton {
            id: makeButton
            anchors.bottom: parent.bottom
            color: Theme.dark700
            text: makeButton.enabled ? ("Make " + (targetBeverage ? targetBeverage.name : "")) : "Cancel"
            textWeight: 700
            enabled: !edgeCoffeeMachineController.isMakingDrink
            onClick: {
                edgeCoffeeMachineController.makeDrink(root.targetBeverage);
            }
        }
    }

    // Placeholder for when no beverage is selected
    TextDefault {
        text: "Select a drink to see details"
        color: "#808080"
        font.pixelSize: 18
        wrapMode: Text.WordWrap
        anchors.centerIn: parent
        visible: root.targetBeverage === null
    }
}
