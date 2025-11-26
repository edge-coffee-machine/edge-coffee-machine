import QtQuick
import "../theme"

Rectangle {
    id: card

    default property list<Item> content

    color: Theme.dark700
    height: 400
    radius: 16
    width: 300

    Item {
        id: contentItem

        anchors.fill: parent
        anchors.margins: 15
        children: content
    }
}
