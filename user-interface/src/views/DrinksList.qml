import QtQuick
import "../components"
import "../theme"
import "../components/text"

Rectangle {
    id: root

    // Signal to request going back to previous view
    signal goBackRequested
    // Signal to notify parent that user switch is requested
    signal switchUserRequested

    color: Theme.dark500
    height: 600
    width: 1024

    Rectangle {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 30
        color: "transparent"

        CloseButton {
            onGoBackRequested: root.goBackRequested()
        }

        Header {
            id: header

            height: 60

            onSwitchUserRequested: root.switchUserRequested()
        }

        Card {
            anchors.top: header.bottom
            height: 450
            width: parent.width

            TextDefault {
                id: title

                color: Theme.white
                font.pixelSize: 22
                text: "Explore <b>our drinks selection.<\b>"
                textFormat: Text.RichText
            }

            TextDefault {
                id: description

                anchors.top: title.bottom
                text: "Discover a curated collection of drinks designed just for you."
            }

            Card {
                anchors.top: description.bottom
                anchors.topMargin: 10
                color: Theme.dark500
                height: 370
                width: parent.width

                Column {
                    spacing: 10

                    TextDefault {
                        font.pixelSize: 18
                        font.weight: 700
                        text: "Coffee Based"
                    }

                    Row {
                        spacing: 10

                        CoffeeTileSquare {
                            imageSource: "../../assets/img/cappuccino.png"
                            name: "Cappuccino"
                            price: "€1.20"
                        }

                        Column {
                            spacing: 10

                            Row {
                                spacing: 10

                                CoffeeTile {
                                    imageSource: "../../assets/img/cappuccino.png"
                                    name: "Cappuccino"
                                    price: "€1.20"
                                }

                                CoffeeTile {
                                    imageSource: "../../assets/img/cappuccino.png"
                                    name: "Cappuccino"
                                    price: "€1.20"
                                }

                                CoffeeTile {
                                    imageSource: "../../assets/img/cappuccino.png"
                                    name: "Cappuccino"
                                    price: "€1.20"
                                }
                            }

                            Row {
                                spacing: 10

                                CoffeeTile {
                                    imageSource: "../../assets/img/cappuccino.png"
                                    name: "Cappuccino"
                                    price: "€1.20"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
