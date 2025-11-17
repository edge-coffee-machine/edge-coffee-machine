import QtQuick 2.15

Card {
    id: root
    width: 270
    height: 420
    color: "#131515" // Aligned with the color of the other cards

    // Expose the ListView's model property as a property of DrinkListPanel
    // This allows main.qml to assign data to it.
    property alias model: drinkListView.model



    Column {
        width: parent.width
        height:300

        spacing: 10

        Column {
            width: parent.width
            id: panelTitle
            Text {
                text: "Just <b>for you</b>"
                textFormat: Text.RichText
                font.pixelSize: 22
                color: "white"
            }

            Text {
                width: parent.width
                text: "Explore a list of drinks created just for you."
                font.pixelSize: 15
                color: "white"
                opacity: 0.7
                font.weight: Font.Light
                wrapMode: Text.WordWrap

            }
        }

        Item {
            height: 268
            width: parent.width
            ListView {
                id: drinkListView
                width: parent.width
                height: parent.height
                spacing: 10
                clip: true // Prevents items from going out of bounds

                // A custom delegate for a better look
                delegate: Component {
                    Card {
                        width: drinkListView.width
                        height: 80
                        color: mouseArea.containsMouse ? "#3A3E40" : "#1A1B1C"

                        Behavior on color { ColorAnimation { duration: 150 } }
                        Row {
                            height: parent.height
                            spacing: 20

                            // Note: In the future we should look at .qrt files for the resources
                            Image {
                                source: "../assets/img/cappuccino.png"
                                width: 60
                                height: 60
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    text: modelData.name // Accesses the 'name' property of the Beverage object
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: 20
                                }

                                Text {
                                    text: "€1.20" // Accesses the 'name' property of the Beverage object
                                    color: "white"
                                    opacity: 0.7
                                    font.pixelSize: 15

                                }
                            }


                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                edgeCoffeeMachineController.selectBeverage(modelData)
                            }
                        }
                    }
                }
            }
        }
        CustomButton {
            anchors.bottom: drinkListView.bottom
            text: "All drinks"
        }
    }
}
