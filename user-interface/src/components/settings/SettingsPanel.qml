import QtQuick 2.15
import "../../theme"
import "../text"
import "../"
import "../buttons"
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Card {
    id: root
    width: 275
    height: 390
    color: Theme.dark500

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

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
                // Object.keys() creates a list of ingredient names from the map
                model: targetBeverage ? Object.keys(
                                            targetBeverage.ingredients) : []
                delegate: SliderInput {
                    label: modelData
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
                height: 130
                color: Theme.dark400
                isLoading: !makeButton.enabled

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
                        edgeCoffeeMachineController.makeDrink(
                                    targetBeverage.name)
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
