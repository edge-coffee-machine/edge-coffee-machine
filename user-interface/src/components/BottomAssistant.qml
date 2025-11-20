import QtQuick 2.15
import "../theme"

Row {
    id: bottomAssistant
    height: 40
    spacing: 20

    Image {
        source: "../../assets/img/voice-wave.png"
        width: 180; height: 90
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: "Brew me an Espresso without sugar..."
        anchors.verticalCenter: parent.verticalCenter
        anchors.bottomMargin: 10

        color: Theme.white
        font.weight: 300
        font.italic: true
        font.pixelSize: 16
    }
}
