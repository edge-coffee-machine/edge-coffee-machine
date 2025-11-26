import QtQuick
import "../theme"
import "text"

Row {
    id: bottomAssistant
    height: 40
    spacing: 20

    Image {
        source: "../../assets/img/voice-wave.png"
        width: 180
        height: 90
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
    }

    TextDefault {
        text: "Brew me an Espresso without sugar..."
        anchors.verticalCenter: parent.verticalCenter
        anchors.bottomMargin: 10

        color: Theme.white
        font.weight: 300
        font.italic: true
        font.pixelSize: 16
    }
}
