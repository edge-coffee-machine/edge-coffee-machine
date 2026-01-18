import QtQuick
import Theme
import "text"

Row {
    id: bottomAssistant

    height: 40
    spacing: 20

    Image {
        anchors.verticalCenter: parent.verticalCenter
        fillMode: Image.PreserveAspectFit
        height: 90
        source: "assets/images/voice-wave.png"
        width: 180
    }

    TextDefault {
        anchors.bottomMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        color: Theme.white
        font.italic: true
        font.pixelSize: 16
        font.weight: 300
        text: "Brew me an Espresso without sugar..."
    }
}
