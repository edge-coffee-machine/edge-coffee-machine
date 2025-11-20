import QtQuick

Rectangle {
    id: root
    property alias text: textItem.text
    property alias textColor: textItem.color
    property bool pressed: false
    signal click()

    height: 40
    color: "#212223"
    width: parent.width
    radius: height / 2

    Text {
        id: textItem
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 16
    }

    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed = true
        onReleased: root.pressed = false
        onClicked: root.click()
    }
}
