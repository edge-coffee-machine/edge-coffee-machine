import QtQuick
import "../components" as Components
import "../components/settings"
import "../theme"

Rectangle {
    id: root

    // Signal to notify parent that drinksList is requested
    signal drinksListRequested

    // Signal to notify parent that user switch is requested
    signal switchUserRequested

    color: Theme.dark500
    height: 600
    width: 1024

    Rectangle {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 30
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

                height: 420
                width: 670

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

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
