import QtQuick
import "../theme"
import "buttons"
import "text"

Card {
    id: root

    // Expose the ListView's model property as a property of DrinkListPanel
    // This allows main.qml to assign data to it.
    property alias model: drinkListView.model

    signal drinksListRequested

    height: 420
    width: 270

    Column {
        height: 300
        spacing: 10
        width: parent.width

        Column {
            id: panelTitle

            width: parent.width

            TextDefault {
                color: Theme.white
                font.pixelSize: 22
                text: "Just <b>for you</b>"
                textFormat: Text.RichText
            }

            TextDefault {
                color: Theme.white
                font.pixelSize: 15
                font.weight: 400
                opacity: 0.7
                text: "Explore a list of drinks created just for you."
                width: parent.width
                wrapMode: Text.WordWrap
            }
        }

        Item {
            height: 268
            width: parent.width

            ListView {
                id: drinkListView

                clip: true // Prevents items from going out of bounds

                height: parent.height
                spacing: 10
                width: parent.width

                // A custom delegate for a better look
                delegate: Component {
                    Card {
                        color: mouseArea.containsMouse ? "#3A3E40" : Theme.dark500
                        height: 80
                        width: drinkListView.width

                        Behavior on color {
                            ColorAnimation {
                                duration: 150
                            }
                        }

                        Row {
                            height: parent.height
                            spacing: 20

                            // Note: In the future we should look at .qrt files for the resources
                            Image {
                                anchors.verticalCenter: parent.verticalCenter
                                height: 60
                                source: "../../assets/img/cappuccino.png"
                                width: 60
                            }

                            Column {
                                anchors.verticalCenter: parent.verticalCenter

                                TextDefault {
                                    color: Theme.white
                                    font.bold: true
                                    font.pixelSize: 20
                                    text: modelData.name // Accesses the 'name' property of the Beverage object
                                }

                                TextDefault {
                                    color: Theme.white
                                    font.pixelSize: 15
                                    opacity: 0.7
                                    text: "€1.20" // Accesses the 'name' property of the Beverage object
                                }
                            }
                        }

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                edgeCoffeeMachineController.selectBeverage(modelData);
                            }
                        }
                    }
                }
            }
        }

        CustomButton {
            text: "All drinks"

            onClick: root.drinksListRequested()
        }
    }
}
