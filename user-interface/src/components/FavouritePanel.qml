import QtQuick
import "../theme"
import "text"

Item {
    id: root

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

    function getImage(targetBeverage) {
        if (targetBeverage === undefined || targetBeverage === null) {
            return "../../assets/img/featured-espresso.png";
        }
        switch (targetBeverage.name) {
        case "Cappuccino":
            return "../../assets/img/featured-cappuccino.png";
        case "Latte":
            return "../../assets/img/featured-latte.png";
        case "Tea":
            return "../../assets/img/featured-tea.png";
        case "Mocha":
            return "../../assets/img/featured-mocha.png";
        case "Americano":
            return "../../assets/img/featured-americano.png";
        case "Milk":
            return "../../assets/img/featured-milk.png";
        default:
            return "../../assets/img/featured-espresso.png";
        }
    }

    height: 410
    width: 350

    TextDefault {
        anchors.horizontalCenter: parent.horizontalCenter
        color: Theme.white
        font.pixelSize: 22
        text: "Here's your <b>favourite.</b>"
        textFormat: Text.RichText
    }

    Card {
        id: favouritePanel

        anchors.bottom: parent.bottom
        anchors.margins: Theme.spacing
        color: Theme.dark500
        height: 350
        radius: 16
        width: 350

        Column {
            anchors.centerIn: parent

            Image {
                id: espressoImg

                fillMode: Image.PreserveAspectFit
                height: 270
                source: root.getImage(root.targetBeverage)
                width: 270
            }

            H1 {
                horizontalAlignment: Text.AlignHCenter
                text: root.targetBeverage ? root.targetBeverage.name : "Espresso"
                width: parent.width
            }
        }
    }
}
