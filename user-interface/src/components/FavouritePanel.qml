import QtQuick 2.15
import "../theme"
import "text"

Item {
    width: 350
    height: 410

    // This property will receive the selected beverage object from main.qml
    property var targetBeverage: null

    function getImage(targetBeverage) {
        if (targetBeverage !== null) {
            if (targetBeverage.name === "Cappuccino") {
                return "../../assets/img/featured-cappuccino.png"
            }

            if (targetBeverage.name === "Latte") {
                return "../../assets/img/featured-latte.png"
            }

            if (targetBeverage.name === "Tea") {
                return "../../assets/img/featured-tea.png"
            }
        }
        return "../../assets/img/espresso.png"
    }

    TextDefault {
        text: "Here's your <b>favourite.</b>"
        color: Theme.white
        font.pixelSize: 22
        textFormat: Text.RichText
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Card {
        id: favouritePanel
        width: 350
        height: 350
        radius: 16
        color: Theme.dark500
        anchors.bottom: parent.bottom
        anchors.margins: Theme.spacing
        Column {
            anchors.centerIn: parent
            Image {
                id: espressoImg
                source: getImage(targetBeverage)
                width: 270
                height: 270
                fillMode: Image.PreserveAspectFit
            }
            H1 {
                text: targetBeverage ? targetBeverage.name : "Espresso"
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
