import QtQuick
import "../text"

Rectangle {
    id: root

    property bool pressed: false
    property string text
    property color textColor

    signal click

    color: "#212223"
    height: 40
    radius: height / 2
    width: 50

    //width: parent.width

    TextDefault {
        id: textItem

        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 16
        font.weight: 700
        text: root.text
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.click()
        onPressed: root.pressed = true
        onReleased: root.pressed = false
    }
}
