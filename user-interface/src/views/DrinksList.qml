import QtQuick 2.15
import "../components"
import "../theme"
import "../components/text"

Rectangle {
    id: root
    width: 1024
    height: 600
    color: Theme.dark500

    Rectangle {
        anchors.fill: parent
        anchors.margins: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "transparent"

        Header {
            id: header
            height: 60
        }

        Card {
            anchors.top: header.bottom
            width: parent.width

            Column {
                TextDefault {
                    text: "Explore <b>our drinks selection.<\b>"
                    color: Theme.white
                    font.pixelSize: 22
                    textFormat: Text.RichText
                }

                TextDefault {
                    text: "Explore a curated collection of drinks designed just for you."
                }

                Card {
                    color: Theme.dark500

                    TextDefault {
                        text: "Coffee Based"
                        font.weight: 700
                        font.pixelSize: 18
                    }

                    Row {
                        height: parent.height
                        spacing: 20

                        // Note: In the future we should look at .qrt files for the resources
                        Image {
                            source: "../../assets/img/cappuccino.png"
                            width: 60
                            height: 60
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            TextDefault {
                                text: modelData.name // Accesses the 'name' property of the Beverage object
                                color: Theme.white
                                font.bold: true
                                font.pixelSize: 20
                            }

                            TextDefault {
                                text: "€1.20" // Accesses the 'name' property of the Beverage object
                                color: Theme.white
                                opacity: 0.7
                                font.pixelSize: 15
                            }
                        }
                    }
                }
            }
        }
    }
}
