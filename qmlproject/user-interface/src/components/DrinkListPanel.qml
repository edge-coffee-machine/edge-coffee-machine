import QtQuick
import Theme
import "buttons"
import "text"

Rectangle {
    id: root

    signal drinksListRequested

    /*ListModel {
        id: drinkListModel

        // Example data; in practice, this will be populated from C++
        ListElement { name: "Cappuccino" }
        ListElement { name: "Latte" }
        ListElement { name: "Espresso" }
        ListElement { name: "Americano" }
        ListElement { name: "Mocha" }
    }*/

    function getImage(drinkName: string): string {
        switch (drinkName) {
        case "Cappuccino":
            return "assets/images/cappuccino.png";
        case "Espresso":
            return "assets/images/espresso.png";
        case "Latte":
            return "assets/images/latte.png";
        case "Mocha":
            return "assets/images/mocha.png";
        case "Americano":
            return "assets/images/americano.png";
        default:
            return "assets/images/missing_texture.png";
        }
    }

    //property Beverage model // Model passed from C++

    color: Theme.dark700
    height: 420
    radius: Theme.cardRadius
    width: 270

    Item {
        anchors.fill: parent
        anchors.margins: Theme.cardMargin

        Column {
            height: 300
            spacing: 10
            width: parent.width

            Column {
                id: panelTitle

                width: parent.width

                Row {
                    TextDefault {
                        color: Theme.white
                        font.pixelSize: 22
                        text: "Just "
                        textFormat: Text.RichText
                    }

                    TextDefault {
                        color: Theme.white
                        font.pixelSize: 22
                        font.weight: 700
                        text: "for you"
                        textFormat: Text.RichText
                    }
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
                id: drinkListContainer

                height: 268
                width: parent.width

                ListView {
                    id: drinkListView

                    clip: true // Prevents items from going out of bounds

                    height: parent.height
                    model: EdgeCoffeeMachine.drinksList
                    spacing: 10
                    width: parent.width

                    // A custom delegate for a better look
                    delegate: Rectangle {
                        color: Theme.dark500
                        height: 80
                        radius: Theme.cardRadius
                        width: drinkListView.width

                        Item {
                            anchors.fill: parent
                            anchors.margins: Theme.cardMargin

                            Row {
                                height: parent.height
                                spacing: 20

                                // Note: In the future we should look at .qrt files for the resources
                                Image {
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 60
                                    source: getImage(EdgeCoffeeMachine.drinksList.data(index).name) //drinkName.name) // Accesses the 'name' property of the Beverage object
                                    width: 60
                                }

                                Column {
                                    anchors.verticalCenter: parent.verticalCenter

                                    TextDefault {
                                        color: Theme.white
                                        font.bold: true
                                        font.pixelSize: 20
                                        text: EdgeCoffeeMachine.drinksList.data(index).name //drinkName.name // Accesses the 'name' property of the Beverage object
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

                                onClicked: {
                                    console.log("Selected drink: " + EdgeCoffeeMachine.drinksList.data(index).name);
                                    EdgeCoffeeMachine.selectBeverage(EdgeCoffeeMachine.drinksList.data(index));
                                }
                            }
                        }
                    }
                }
            }
        }

        CustomButton {
            anchors.bottom: parent.bottom
            text: "All drinks"
            width: parent.width

            onClick: root.drinksListRequested()
        }
    }
}
