import QtQuick
import Theme

Rectangle {
    id: card

    property int brewingTime
    property Beverage targetBeverage

    property bool isLoading: EdgeCoffeeMachine.isMakingDrink
    property int waveBottomMargins: 25

    clip: true
    color: Theme.dark700
    height: 400
    radius: Theme.cardRadius
    width: 300

    // Mask over the laoding animation
    Image {
        height: parent.height
        source: "assets/images/border.png"
        width: parent.width
        z: 1
    }

    Image {
        id: wave

        anchors.bottom: progressFill.top
        anchors.bottomMargin: -card.waveBottomMargins
        anchors.horizontalCenter: parent.horizontalCenter
        height: 90
        source: "assets/images/wave.png"
        width: card.isLoading ? (parent.width + 200) : parent.width + 100

        SequentialAnimation on rotation {

            // Make it spin forever
            loops: Animation.Infinite
            // Run the animation automatically
            running: card.isLoading

            RotationAnimation {
                duration: card.brewingTime / 6 // Speed of the tilt (0.5 seconds)
                // Use Easing.InOutQuad for a smoother start and stop at the edges
                easing.type: Easing.InOutQuad
                // Start where the previous loop ended (or at 0 on the first run)
                from: -5
                to: 5
            }

            RotationAnimation {
                duration: card.brewingTime / 6 // Must match the first duration for symmetry
                easing.type: Easing.InOutQuad
                // Starts where the previous animation ended (20 degrees)
                from: 5
                to: -5
            }
        }
        Behavior on width {
            NumberAnimation {
                duration: card.brewingTime
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: progressFill

        anchors.bottom: parent.bottom
        anchors.bottomMargin: -wave.height + card.waveBottomMargins
        color: Theme.accent // Or a distinct color like green
        height: card.isLoading ? (parent.height + 35) : 10
        opacity: 1
        radius: 16
        width: parent.width

        Behavior on height {
            NumberAnimation {
                duration: card.brewingTime
                easing.type: Easing.InOutQuad
            }
        }
    }

    Item {
        id: contentItem

        anchors.fill: parent
        anchors.margins: 15



        TextDefault {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 6
            color: Theme.white
            font.bold: true
            font.pixelSize: 40
            text: isLoading ? "€ 1.20" : "Brewing"
        }

        CustomButton {
            id: makeButton

            width: parent.width
            anchors.bottom: parent.bottom
            color: Theme.dark700
            text: !EdgeCoffeeMachine.isMakingDrink ? ("Make " + (targetBeverage ? targetBeverage.name : "")) : "Cancel"

            onClick: {
                console.log("Make drink button clicked");
                if (!EdgeCoffeeMachine.isMakingDrink) {
                    EdgeCoffeeMachine.makeDrink(targetBeverage);
                } else {
                    EdgeCoffeeMachine.stopBrewing()
                }


            }
        }
    }
}
