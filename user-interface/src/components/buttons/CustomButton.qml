import QtQuick
import "../text"

Rectangle {
    id: root

    property bool pressed: false
    property alias text: textItem.text
    property alias textColor: textItem.color
    property alias textWeight: textItem.font.weight

    signal click

    color: "#212223"
    height: 40
    radius: height / 2
    width: parent.width

    TextDefault {
        id: textItem

        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 16
        font.weight: root.textWeight
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.click()
        onPressed: root.pressed = true
        onReleased: root.pressed = false
    }
}
