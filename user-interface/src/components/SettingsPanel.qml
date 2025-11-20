import QtQuick 2.15
import "../theme"
import "text"
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

            Text {
                text: "Settings"
                color: Theme.white
                font.pixelSize: 24
                font.bold: true
            }

            Subtitle {
                text: "Customize your drink."
            }

            Text {
                text: "Ingredients:"
                color: "#A0A0A0"
                font.pixelSize: 16
                Layout.topMargin: 10
            }

            // We use a Repeater to iterate over the ingredients map
            ColumnLayout {
                id: ingredientsRepeater
                Layout.fillWidth: true
                Layout.leftMargin: 20

                Repeater {
                    // Object.keys() creates a list of ingredient names from the map
                    model: targetBeverage ? Object.keys(targetBeverage.ingredients) : []

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: modelData // 'modelData' here is the ingredient name (the key)
                            color: "#E0E0E0"
                            font.pixelSize: 18
                        }
                        Item { Layout.fillWidth: true } // Spacer
                        Text {
                            // We access the map value using the key
                            text: targetBeverage.ingredients[modelData] + "g"
                            color: "#A0A0A0"
                            font.pixelSize: 16
                        }
                    }
                }
            }

        Item {
            Layout.fillHeight: true
        }

        CardWithLoading {
            Layout.fillWidth: true
            height: 130
            color: Theme.dark400
            isLoading: !makeButton.enabled

            Text {
                text: makeButton.enabled ? "€ 1.20" : "Brewing"
                font.pixelSize: 40
                font.bold: true
                color: Theme.white
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top // Ensure it stays at top
            }

            Item {
                width: parent.width
                height: 40
                anchors.bottom: parent.bottom
                Button {
                    id: makeButton
                    text: "Make " + (targetBeverage ? targetBeverage.name : "")
                    anchors.fill: parent
                    // Disable the button while the machine is busy
                    enabled: !edgeCoffeeMachineController.isMakingDrink
                    onClicked: {
                        edgeCoffeeMachineController.makeDrink(targetBeverage.name)
                    }
                    background: Rectangle {
                        // Colors more consistent with the theme
                        color: Theme.dark700
                        radius: parent.height
                    }
                    contentItem: Item {
                        anchors.fill: parent
                        Text {
                            text: makeButton.enabled ? makeButton.text : "Cancel"
                            color: Theme.white
                            font.bold: true
                            font.pixelSize: 18
                            anchors.centerIn: parent
                        }
                    }
                }
            }
        }

        // Placeholder for when no beverage is selected
        Text {
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
