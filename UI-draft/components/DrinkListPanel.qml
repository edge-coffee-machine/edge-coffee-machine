import QtQuick 2.15

Rectangle {
    id: root
    width: 380
    height: 420
    color: "#131515" // Aligned with the color of the other cards
    radius: 10

    // Expose the ListView's model property as a property of DrinkListPanel
    // This allows main.qml to assign data to it.
    property alias model: drinkListView.model

    Text {
        id: panelTitle
        text: "Available Drinks"
        color: "#E0E0E0"
        font.pixelSize: 20
        font.bold: true
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ListView {
        id: drinkListView
        anchors.top: panelTitle.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 20 // Side margins
        anchors.topMargin: 15
        anchors.bottomMargin: 10
        spacing: 5
        clip: true // Prevents items from going out of bounds

        // A custom delegate for a better look
        delegate: Component {
            Rectangle {
                width: drinkListView.width
                height: 50
                color: mouseArea.containsMouse ? "#3A3E40" : "#2A2E30"
                radius: 8

                Behavior on color { ColorAnimation { duration: 150 } }

                Text {
                    text: modelData.name // Accesses the 'name' property of the Beverage object
                    color: "white"
                    font.pixelSize: 18
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        edgeCoffeeMachineController.selectBeverage(modelData.name)
                    }
                }
            }
        }
    }
}