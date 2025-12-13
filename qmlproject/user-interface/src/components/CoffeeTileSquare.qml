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
    height: 170
    radius: Theme.cardRadius
    width: 170

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

    Column {
        height: parent.height
        spacing: 10
        width: parent.width

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            height: 90
            source: getImage(root.drink.name)
            width: 90
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            TextDefault {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.white
                font.bold: true
                font.pixelSize: 20
                text: root.drink.name
            }

            TextDefault {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.white
                font.pixelSize: 15
                opacity: 0.7
                text: root.price
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