import QtQuick

Rectangle {
    id: card
    width: 300; height: 400
    radius: 16

    // Allow placing arbitrary children inside the card by making 'content' the
    // default property that maps to an internal Item's data.
    default property alias content: contentItem.data

    // Internal content holder — keeps children inside card padding
    Item {
        id: contentItem
        anchors.fill: parent
        anchors.margins: 15
    }
}
