import QtQuick
import "text"
import Theme

Rectangle {
    id: root

    // Props
    signal goBackRequested
    property Beverage drink
    property string price: "€1.20"

    color: Theme.dark400
    height: 80
    radius: Theme.cardRadius
    width: 235

    function getImage(drinkName: string): string {
        switch (drinkName) {
        case "Cappuccino":
            return "assets/images/cappuccino.png";
        case "Espresso":
            return "assets/images/espresso.png";
        case "Latte":
            return "assets/images/latte.png";
        default:
            return "assets/images/missing_texture.png";
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: Theme.cardMargin

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20

            Image {
                anchors.verticalCenter: parent.verticalCenter
                height: 60
                source: getImage(root.drink.name)
                width: 60
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 5

                TextDefault {
                    color: Theme.white
                    font.bold: true
                    font.pixelSize: 20
                    text: drink.name
                }

                TextDefault {
                    color: Theme.white
                    font.pixelSize: 15
                    opacity: 0.7
                    text: root.price
                }
            }
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: {
            console.log("Selected drink: " + root.drink.name);
            EdgeCoffeeMachine.selectBeverage(root.drink);
            root.goBackRequested()
        }
    }
}