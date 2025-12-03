import QtQuick
import Theme
import "text"
import "../placeholders"

Item {
    id: root

    // This property will receive the selected beverage object from main.qml
    property Beverage targetBeverage: null


    function getImage(targetBeverage: Beverage): string {
        /*
        if (targetBeverage == undefined || targetBeverage == null) {
            return "assets/images/featured-espresso.png";
        }*/
        switch (targetBeverage.name) {
        case "Cappuccino":
            return "assets/images/featured-cappuccino.png";
        case "Latte":
            return "assets/images/missing_texture.png";
        case "Tea":
            return "assets/images/missing_texture.png";
        case "Mocha":
            return "assets/images/featured-mocha.png";
        case "Americano":
            return "assets/images/featured-americano.png";
        case "Milk":
            return "assets/images/featured-milk.png";
        default:
            return "assets/images/featured-espresso.png";
        }
    }

    height: 410
    width: 350

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        TextDefault {
            color: Theme.white
            font.pixelSize: 22
            text: "Here's your "
        }

        TextDefault {
            color: Theme.white
            font.pixelSize: 22
            font.weight: 600
            text: "favourite."
        }
    }
    

    Rectangle {
        id: favouritePanel

        anchors.bottom: parent.bottom
        anchors.margins: Theme.spacing
        color: Theme.dark500
        height: 350
        radius: Theme.cardRadius
        width: 350

        Item {
            anchors.fill: parent
            anchors.margins: Theme.cardMargin

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
                    text: root.targetBeverage ? root.targetBeverage.name : "Cappuccino"
                    width: parent.width
                }
            }
        }
    }
}