import QtQuick 2.15

Item {
    id: bottomAssistant
    width: parent ? parent.width : 1024
    height: 40

    Text {
        text: "Brew me an Espresso without sugar..."
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#D9A569"
        font.pixelSize: 16
    }
}
