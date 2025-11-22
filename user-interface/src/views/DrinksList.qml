import QtQuick 2.15
import "../components"
import "../theme"
import "../components/text"

Rectangle {
    id: root
    width: 1024
    height: 600
    color: Theme.dark500

    // Signal to request going back to previous view
    signal goBackRequested
    // Signal to notify parent that user switch is requested
    signal switchUserRequested

    Rectangle {
        anchors.fill: parent
        anchors.margins: 30
        anchors.horizontalCenter: parent.horizontalCenter
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
            width: parent.width
            height: 450

            TextDefault {
                id: title
                text: "Explore <b>our drinks selection.<\b>"
                color: Theme.white
                font.pixelSize: 22
                textFormat: Text.RichText
            }

            TextDefault {
                id: description
                text: "Discover a curated collection of drinks designed just for you."
                anchors.top: title.bottom
            }

            Card {
                anchors.top: description.bottom
                anchors.topMargin: 10
                color: Theme.dark500
                width: parent.width
                height: 370

                Column {
                    spacing: 10
                    TextDefault {
                        text: "Coffee Based"
                        font.weight: 700
                        font.pixelSize: 18
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
