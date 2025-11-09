// Main.qml (rearranged into components)
import QtQuick 2.15
import "components" as Components
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 1024
    height: 600
    color: "#17191A"

    // Top header (moved to component)
    Components.Header {
        id: header
        width: parent.width
        height: 60
    }

    // Main content as three components
    Row {
        id: mainRow
        anchors.top: header.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20

        Components.Card {
            id: card1
            color: "#131515"
            width: 600
            height: 420

            Components.SettingsPanel {
                id: settingsPanel
                anchors.fill: parent // Make the panel fill the Card
                targetBeverage: edgeCoffeeMachineController.selectedBeverage // Connects the selected beverage to the panel
            }
        }


        Components.DrinkListPanel {
            id: drinkListPanel
            model: edgeCoffeeMachineController.beverages // Passes the list of beverages from C++ to the panel's model
        }
    }



    // Bottom assistant (moved to component)
    Components.BottomAssistant {
        id: bottomAssistant
        width: parent.width
        anchors.bottom: parent.bottom
    }
}
