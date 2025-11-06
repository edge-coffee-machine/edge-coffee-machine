import QtQuick 2.15

Item {
    id: header
    width: parent ? parent.width : 1024
    height: 60

    Text {
        text: "Good Morning,"
        font.pixelSize: 28
        font.bold: true
        color: "white"
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: "Matteo"
        font.pixelSize: 28
        color: "#D9A569"
        anchors.left: parent.left
        anchors.leftMargin: 250
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        height: 20
        width: 1
        anchors.left: parent.left
        anchors.leftMargin: 350
        anchors.verticalCenter: parent.verticalCenter
        opacity: 0.2
    }

    Rectangle {
        id: notYouBtn
        width: 80; height: 28
        radius: 14
        color: "#2b2b2b"
        anchors.left: parent.left
        anchors.leftMargin: 365
        anchors.verticalCenter: parent.verticalCenter

        Text {
            text: "Not you?"
            color: "#D9A569"
            anchors.centerIn: parent
            font.pixelSize: 14
        }

        MouseArea {
            anchors.fill: parent
            onClicked: console.log("Switch user")
        }
    }
}
