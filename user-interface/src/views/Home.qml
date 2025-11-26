import QtQuick
import "../components" as Components
import "../components/settings"
import "../theme"

Rectangle {
    id: root
    width: 1024
    height: 600
    color: Theme.dark500

    // Signal to notify parent that user switch is requested
    signal switchUserRequested

    // Signal to notify parent that drinksList is requested
    signal drinksListRequested

    Rectangle {
        anchors.fill: parent
        anchors.margins: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "transparent"

        Components.Header {
            id: header
            height: 60
            onSwitchUserRequested: root.switchUserRequested()
        }

        // Main content as three components
        Row {
            id: mainRow
            anchors.top: header.bottom
            spacing: 20

            Components.Card {
                id: card1
                width: 670
                height: 420
                Row {
                    spacing: 15

                    Components.FavouritePanel {
                        id: favouritePanel
                        targetBeverage: edgeCoffeeMachineController.selectedBeverage
                    }

                    SettingsPanel {
                        id: settingsPanel
                        targetBeverage: edgeCoffeeMachineController.selectedBeverage // Connects the selected beverage to the panel
                    }
                }
            }

            Components.DrinkListPanel {
                id: drinkListPanel
                model: edgeCoffeeMachineController.beverages // Passes the list of beverages from C++ to the panel's model
                onDrinksListRequested: root.drinksListRequested()
            }
        }

        // Bottom assistant (moved to component)
        Components.BottomAssistant {
            id: bottomAssistant
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
        }
    }
}
