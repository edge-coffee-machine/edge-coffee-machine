import QtQuick
import "../components" as Components
import Theme

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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: "transparent"
        height: parent.height - 60
        width: parent.width - 60

        Components.Header {
            id: header

            height: 60
            width: parent.width

            onSwitchUserRequested: root.switchUserRequested()
        }

        // Main content as three components
        Row {
            id: mainRow

            anchors.top: header.bottom
            spacing: 15

            Rectangle {
                id: card

                color: Theme.dark700
                height: 420
                radius: Theme.cardRadius
                width: 680

                Item {
                    anchors.fill: parent
                    anchors.margins: Theme.cardMargin

                    Row {
                        spacing: 15

                        Components.FavouritePanel {
                            id: favouritePanel

                            targetBeverage: EdgeCoffeeMachine.selectedBeverage
                        }

                        SettingsPanel {
                            id: settingsPanel

                            targetBeverage: EdgeCoffeeMachine.selectedBeverage // Connects the selected beverage to the panel
                        }
                    }
                }
            }

            Components.DrinkListPanel {
                id: drinkListPanel

                // model: EdgeCoffeeMachine.getPopularBeverages() // Passes the list of beverages from C++ to the panel's model

                onDrinksListRequested: root.drinksListRequested()
            }
        }

        // Bottom assistant

        Components.BottomAssistant {
            id: bottomAssistant

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
