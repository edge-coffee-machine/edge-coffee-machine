import QtQuick 2.15
import "../theme"
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
                text: targetBeverage ? targetBeverage.name : ""
                color: Theme.white
                font.pixelSize: 24
                font.bold: true
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

        Card {
            Layout.fillWidth: true
            height: 130
            color: Theme.dark400
            Text {
                text: "€ 1.20"
                font.pixelSize: 40
                font.bold: true
                color: Theme.white
                anchors.horizontalCenter: parent.horizontalCenter
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
                        color: makeButton.down ? "#4A4E50" : (makeButton.hovered ? "#3A3E40" : Theme.dark500)
                        radius: parent.height
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }
                    contentItem: Item {
                        anchors.fill: parent
                        Text {
                            text: makeButton.text
                            color: Theme.white
                            font.bold: true
                            font.pixelSize: 18
                            anchors.centerIn: parent
                            visible: makeButton.enabled // Show text only when the button is active
                        }
                        BusyIndicator {
                            anchors.centerIn: parent
                            running: !makeButton.enabled // The indicator runs when the button is disabled
                            visible: running
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
