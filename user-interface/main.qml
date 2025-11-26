import QtQuick

Rectangle {
    id: root

    // Navigation stack to track view history
    property var viewStack: []

    height: 600
    width: 1024

    // Container for the current view
    Loader {
        id: viewLoader

        anchors.fill: parent
        source: "src/views/Home.qml"

        // Use Connections to bind to optional signals on the loaded view.
        // If a signal is not present on the target, QML silently ignores the handler
        // and avoids missing-property warnings.
        Connections {
            // Triggered by views that expose `signal drinksListRequested()`.
            function onDrinksListRequested() {
                // Open the Drinks List view and push current view to the stack.
                root.viewStack.push(viewLoader.source);
                viewLoader.source = "src/views/DrinksList.qml";
            }

            // Triggered by views that expose `signal goBackRequested()`.
            function onGoBackRequested() {
                // Navigate back to the previous view, if any.
                if (root.viewStack.length > 0) {
                    viewLoader.source = root.viewStack.pop();
                }
            }

            // Triggered by views that expose `signal switchUserRequested()`.
            function onSwitchUserRequested() {
                // Open the Users Panel and push current view to the stack.
                root.viewStack.push(viewLoader.source);
                viewLoader.source = "src/views/UsersPanel.qml";
            }

            ignoreUnknownSignals: true
            target: viewLoader.item // `target` becomes the currently loaded view instance.
        }
    }
}
