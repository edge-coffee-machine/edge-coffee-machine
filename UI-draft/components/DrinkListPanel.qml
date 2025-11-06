import QtQuick 2.15

Card {
    width: 270
    height: 420
    color: "#131515"

    Column {
        id: drinkListPanel
        width: 250; height: 400
        anchors.fill: parent
        spacing: 10

        Column {
            id: textColumn
            Text {
                text: "Just <b>for you</b>"
                textFormat: Text.RichText
                font.pixelSize: 18
                color: "white"
            }

            Text {
                text: "Explore a list of drinks created just for you."
                font.pixelSize: 10
                color: "white"
                font.weight: Font.Light

            }
        }



        // Scrollable list simulation
        Flickable {
            width: parent.width
            height: drinkListPanel.height - button.height - textColumn.height - drinkListPanel.spacing * 2
            contentHeight: columnContent.height
            clip: true

            Column {
                id: columnContent
                width: parent.width
                spacing: 10

                Repeater {
                    model: [
                        { name: "Cappuccino", price: "€ 2,20" },
                        { name: "Mocaccino", price: "€ 1,30" },
                        { name: "Latte", price: "€ 1,00" },
                        { name: "Lungo", price: "€ 1,50" },
                        { name: "Cappuccino", price: "€ 2,20" },
                        { name: "Mocaccino", price: "€ 1,30" },
                        { name: "Latte", price: "€ 1,00" },
                        { name: "Lungo", price: "€ 1,50" }
                    ]
                    delegate: Rectangle {
                        width: parent.width
                        height: 50
                        radius: 8
                        color: "#1A1B1C"

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 10

                            Image {
                                source: "://assets/img/cappuccino.png"
                                width: 40; height: 40
                                fillMode: Image.PreserveAspectFit
                            }
                            Column {
                                spacing: 2
                                Text { text: modelData.name; color: "white"; font.pixelSize: 16; font.bold: true }
                                Text { text: modelData.price; color: "white"; font.pixelSize: 14; font.weight: Font.Light }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: console.log("Selected", modelData.name)
                        }
                    }
                }
            }
        }

        Rectangle {
            id: button
            width: parent.width
            height: 40
            radius: 20
            color: "#212223"
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: "All drinks"
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 16
            }
        }
    }
}

