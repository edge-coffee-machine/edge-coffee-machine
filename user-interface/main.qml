// Main.qml (rearranged into components)
import QtQuick 2.15

Rectangle {
    id: root
    width: 1024
    height: 600

    // Navigation stack to track view history
    property var viewStack: []

    // Container for the current view
    Loader {
        id: viewLoader
        anchors.fill: parent
        source: "src/views/Home.qml"

        onLoaded: {
            // Connect the switchUser signal if the loaded view has it
            if (item && item.switchUserRequested) {
                item.switchUserRequested.connect(openUsersPanel)
            }

            if (item && item.drinksListRequested) {
                item.drinksListRequested.connect(openDrinksList)
            }

            // Connect the goBack signal if it exists (for UsersPanel)
            if (item && item.goBackRequested) {
                item.goBackRequested.connect(goBack)
            }
        }
    }

    // Function to open UsersPanel and save current view to stack
    function openUsersPanel() {
        viewStack.push(viewLoader.source)
        viewLoader.source = "src/views/UsersPanel.qml"
    }

    function openDrinksList() {
        viewStack.push(viewLoader.source)
        viewLoader.source = "src/views/DrinksList.qml"
    }

    // Function to go back to previous view
    function goBack() {
        if (viewStack.length > 0) {
            viewLoader.source = viewStack.pop()
        }
    }
}
