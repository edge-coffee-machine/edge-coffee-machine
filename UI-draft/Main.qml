// Main.qml (rearranged into components)
import QtQuick 2.15
import "components" as Components

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

            Row {
                id: mainComponentRow
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                spacing: 10

            Components.FavouritePanel { id: favouritePanel }
            Components.SettingsPanel { id: settingsPanel }
            }
        }


        Components.DrinkListPanel { id: drinkListPanel }
    }



    // Bottom assistant (moved to component)
    Components.BottomAssistant {
        id: bottomAssistant
        width: parent.width
        anchors.bottom: parent.bottom
    }
}
