import QtQuick
import "../theme"

Rectangle {
    id: root
    property alias text: textItem.text
    property alias textColor: textItem.color
    property bool pressed: false
    signal click()

    width: textItem.width + 20
    height: textItem.height + 10
    radius: height / 2
    color: pressed ? "#1f1f1f" : Theme.gray

    Text {
        id: textItem
        anchors.centerIn: parent
        color: Theme.accent
    }

    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed = true
        onReleased: root.pressed = false
        onClicked: root.click()
    }
}
