import QtQuick 2.15
import QtQuick.Layouts 1.15
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

    onTargetBeverageChanged: {
        // Force re-render to update the values
        sliderGroup.model = null
        sliderGroup.model = ["Coffee", "Cocoa", "Water", "Foam", "Milk"]
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // This content is only visible when a beverage is selected
        ColumnLayout {
            Layout.fillWidth: true
            visible: targetBeverage !== null

            TextDefault {
                text: "Settings"
                color: Theme.white
                font.pixelSize: 24
                font.bold: true
            }

            Subtitle {
                text: "Customize your drink."
            }
            Repeater {
                id: sliderGroup
                delegate: SliderInput {
                    label: modelData
                    maximumValue: {
                        if (!targetBeverage)
                            return 0
                        switch (modelData) {
                        case "Coffee":
                            return targetBeverage.maxCoffeeBeans
                        case "Cocoa":
                            return targetBeverage.maxCocoaPowder
                        case "Water":
                            return targetBeverage.maxWater
                        case "Foam":
                            return targetBeverage.maxFoam
                        case "Milk":
                            return targetBeverage.maxMilk
                        default:
                            return 0
                        }
                    }

                    unitOfM: {
                        if (modelData == "Cocoa")
                            return "g"
                        else
                            return "ml"
                    }

                    // Bind value based on the ingredient name
                    value: {
                        if (!targetBeverage)
                            return 0
                        switch (modelData) {
                        case "Coffee":
                            return targetBeverage.coffeeBeans
                        case "Cocoa":
                            return targetBeverage.cocoaPowder
                        case "Water":
                            return targetBeverage.water
                        case "Foam":
                            return targetBeverage.foam
                        case "Milk":
                            return targetBeverage.milk
                        default:
                            return 0
                        }
                    }

                    // Update the beverage property when slider changes
                    onValueChanged: {
                        if (!targetBeverage)
                            return
                        switch (modelData) {
                        case "Coffee":
                            targetBeverage.coffeeBeans = value
                            break
                        case "Cocoa":
                            targetBeverage.cocoaPowder = value
                            break
                        case "Water":
                            targetBeverage.water = value
                            break
                        case "Foam":
                            targetBeverage.foam = value
                            break
                        case "Milk":
                            targetBeverage.milk = value
                            break
                        }
                    }
                }
            }

            // We use a Repeater to iterate over the ingredients map
            ColumnLayout {
                id: ingredientsRepeater
                Layout.fillWidth: true
                Layout.leftMargin: 20
            }

            Item {
                Layout.fillHeight: true
            }

            CardWithLoading {
                Layout.fillWidth: true
                implicitHeight: 130
                color: Theme.dark400
                isLoading: !makeButton.enabled
                brewingTime: targetBeverage.brewingTime()

                TextDefault {
                    text: makeButton.enabled ? "€ 1.20" : "Brewing"
                    font.pixelSize: 40
                    font.bold: true
                    color: Theme.white
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top // Ensure it stays at top
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
                        edgeCoffeeMachineController.makeDrink(targetBeverage)
                    }
                }
            }

            // Placeholder for when no beverage is selected
            TextDefault {
                text: "Select a drink to see details"
                color: "#808080"
                font.pixelSize: 18
                wrapMode: Text.WordWrap
                Layout.alignment: Qt.AlignCenter // Use Layout.alignment to center
                visible: targetBeverage === null
            }
        }
    }
}
