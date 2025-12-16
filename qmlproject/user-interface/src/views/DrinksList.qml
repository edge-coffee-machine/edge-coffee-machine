import QtQuick
import "../components"
import Theme
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
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 30
        color: "transparent"

        CloseButton {
            id: closeBtn
            onGoBackRequested: root.goBackRequested()
            anchors {
                top: parent.top
                right: parent.right
            }
        }

        Header {
            id: header
            anchors {
                top: closeBtn.bottom
            }

            height: 60
            width: parent.width

            onSwitchUserRequested: root.switchUserRequested()
        }
        
        Rectangle {
            anchors.top: header.bottom
            color: Theme.dark700
            height: 450
            radius: Theme.cardRadius
            width: parent.width

            Item {
                anchors.fill: parent
                anchors.margins: Theme.cardMargin

                Column {
                    spacing: 10
                    width: parent.width
                    Row {
                        TextDefault {
                            color: Theme.white
                            font.pixelSize: 22
                            text: "Explore "
                        }

                        TextDefault {
                            color: Theme.white
                            font.pixelSize: 22
                            text: "our drinks selection"
                            font.weight: 700
                        }
                    }

                    TextDefault {
                    id: description
                    font.pixelSize: 16
                    opacity: 0.7
                    text: "Discover a curated collection of drinks designed just for you."
                }

                Rectangle {
                    color: Theme.dark500
                    height: 355
                    radius: Theme.cardRadius
                    width: parent.width

                    Item {
                        anchors.fill: parent
                        anchors.margins: Theme.cardMargin

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
                                    onGoBackRequested: root.goBackRequested()
                                    drink: EdgeCoffeeMachine.drinksList.data(0)
                                    price: "€1.20"
                                }

                                Column {
                                    spacing: 10

                                    Row {
                                        spacing: 10

                                        CoffeeTile {
                                            onGoBackRequested: root.goBackRequested()
                                            drink: EdgeCoffeeMachine.drinksList.data(1)
                                            price: "€1.20"
                                        }

                                        CoffeeTile {
                                            onGoBackRequested: root.goBackRequested()
                                            drink: EdgeCoffeeMachine.drinksList.data(2)
                                            price: "€1.20"
                                        }

                                        CoffeeTile {
                                            onGoBackRequested: root.goBackRequested()
                                            drink: EdgeCoffeeMachine.drinksList.data(3)
                                            price: "€1.20"
                                        }
                                    }

                                    Row {
                                        spacing: 10

                                        CoffeeTile {
                                            onGoBackRequested: root.goBackRequested()
                                            drink: EdgeCoffeeMachine.drinksList.data(4)
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
        }
    }
}