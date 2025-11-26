import QtQuick
import "../theme"

Rectangle {
    id: card

    // Allow placing arbitrary children inside the card by making 'content' the
    // default property that maps to an internal Item's data.
    default property alias content: contentItem.data

    color: Theme.dark700
    height: 400
    radius: 16
    width: 300

    // Internal content holder — keeps children inside card padding
    Item {
        id: contentItem

        anchors.fill: parent
        anchors.margins: 15
    }
}
