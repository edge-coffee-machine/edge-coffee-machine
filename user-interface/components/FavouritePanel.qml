import QtQuick 2.15
Column {
    width: 350; height: 400
    Text {
        text: "Here's your <b>favourite.<\b>"
        color: "white"
        font.pixelSize: 18
        textFormat: Text.RichText
        anchors.horizontalCenter: parent.horizontalCenter
        y: 10
    }


    Rectangle {
        id: favouritePanel
        width: 350; height: 350
        radius: 16
        color: "#1a1a1a"
        anchors.bottom: parent.bottom

        Column {
            anchors.centerIn: parent
            spacing: 20

            // The source will probabily need to be adjusted.
            Image {
                id: espressoImg
                source: "://assets/img/cappuccino.png"
                width: 180; height: 180
                fillMode: Image.PreserveAspectFit
            }
            Text {
                text: "Espresso"
                color: "white"
                font.bold: true
                font.pixelSize: 24
            }
        }
    }
}
