import QtQuick
import "../theme"

Rectangle {
    id: root

    property bool pressed: false
    property alias text: textItem.text
    property alias textColor: textItem.color

    signal click

    color: pressed ? "#1f1f1f" : Theme.gray
    height: textItem.height + 10
    radius: height / 2
    width: textItem.width + 20

    Text {
        id: textItem

        anchors.centerIn: parent
        color: Theme.accent
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.click()
        onPressed: root.pressed = true
        onReleased: root.pressed = false
    }
}
