import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Card {
    id: root
    color: "#1A1B1C"

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        // This content is only visible when a beverage is selected
        ColumnLayout {
            Layout.fillWidth: true
            visible: targetBeverage !== null

            Text {
                text: targetBeverage ? targetBeverage.name : ""
                color: "white"
                font.pixelSize: 24
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Ingredients:"
                color: "#A0A0A0"
                font.pixelSize: 16
                Layout.topMargin: 10
            }

            // Ingredients display
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 20

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Coffee Beans"
                        color: "#E0E0E0"
                        font.pixelSize: 18
                    }
                    Item { Layout.fillWidth: true } 
                    Text {
                        text: targetBeverage ? (targetBeverage.minCoffeeBeans + targetBeverage.coffeeBeans * (targetBeverage.maxCoffeeBeans - targetBeverage.minCoffeeBeans)).toFixed(2) + "g" : "0g"
                        color: "#A0A0A0"
                        font.pixelSize: 16
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Cocoa Powder"
                        color: "#E0E0E0"
                        font.pixelSize: 18
                    }
                    Item { Layout.fillWidth: true }
                    Text {
                        text: targetBeverage ? (targetBeverage.minCocoaPowder + targetBeverage.cocoaPowder * (targetBeverage.maxCocoaPowder - targetBeverage.minCocoaPowder)).toFixed(2) + "g" : "0g"
                        color: "#A0A0A0"
                        font.pixelSize: 16
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Water"
                        color: "#E0E0E0"
                        font.pixelSize: 18
                    }
                    Item { Layout.fillWidth: true } 
                    Text {
                        text: targetBeverage ? (targetBeverage.minWater + targetBeverage.water * (targetBeverage.maxWater - targetBeverage.minWater)).toFixed(2) + "ml" : "0ml"
                        color: "#A0A0A0"
                        font.pixelSize: 16
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Foam"
                        color: "#E0E0E0"
                        font.pixelSize: 18
                    }
                    Item { Layout.fillWidth: true } 
                    Text {
                        text: targetBeverage ? (targetBeverage.minFoam + targetBeverage.foam * (targetBeverage.maxFoam - targetBeverage.minFoam)).toFixed(2) + "ml" : "0ml"
                        color: "#A0A0A0"
                        font.pixelSize: 16
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Milk"
                        color: "#E0E0E0"
                        font.pixelSize: 18
                    }
                    Item { Layout.fillWidth: true } // Espaciador
                    Text {
                        text: targetBeverage ? (targetBeverage.minMilk + targetBeverage.milk * (targetBeverage.maxMilk - targetBeverage.minMilk)).toFixed(2) + "ml" : "0ml"
                        color: "#A0A0A0"
                        font.pixelSize: 16
                    }
                }
            }

        Item {
            Layout.fillHeight: true
        }

        Card {
            Layout.fillWidth: true
            height: 150
            color: "#212223"
            Text {
                y: 20
                text: "€ 1.20"
                font.pixelSize: 40
                font.bold: true
                color: "white"
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
                        color: makeButton.down ? "#4A4E50" : (makeButton.hovered ? "#3A3E40" : "#1A1B1C")
                        radius: parent.height
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }
                    contentItem: Item {
                        anchors.fill: parent
                        Text {
                            text: makeButton.text
                            color: "white"
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
