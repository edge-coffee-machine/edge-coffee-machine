import QtQuick 2.15

Card {
    id: settingsPanel
    width: 220; height: 400
    radius: 16
    color: "#1a1a1a"

    Column {
        spacing: 5

        Column {
            Text {
                text: "Settings"
                font.pixelSize: 20
                font.bold: true
                color: "white"
            }

            Text {
                text: "Customize your drink."
                font.weight: Font.Light
                font.pixelSize: 10
                color: "white"

            }
        }



        // Example slider row
        Repeater {
            model: ["Foam", "Milk", "Water", "Powder", "Coffee"]
            delegate: Item {
                width: 200; height: 40
                    Text {
                        text: modelData
                        color: "white"
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                    }
                        Rectangle {
                            id: bar
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            width: 100; height: 6
                            radius: 3
                            color: "#17191A"
                            Rectangle {
                                width: 50; height: 6
                                radius: 3
                                color: "white"
                            }
                        }
                        Rectangle {
                            width: parent.width
                            height: 1.2
                            opacity: 0.05
                            radius: 2
                        }





            }
        }
    }

    Card {
        height: 100
        width: 200
        anchors.bottom: parent.bottom
        color: "#212223"
        Text {
            text: "€ 1.20"
            color: "white"
            font.bold: true
            font.pixelSize: 28
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CustomButton {
            text: "Brew now"
            color: "#17191A"
            onClick: console.log("Brew Espresso")
            anchors.bottom: parent.bottom
        }
    }
}
