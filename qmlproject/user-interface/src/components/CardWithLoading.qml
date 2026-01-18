import QtQuick
import Theme

Rectangle {
    id: card

    property int brewingTime
    property bool isLoading: EdgeCoffeeMachine.isMakingDrink
    property Beverage targetBeverage
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
                duration: 1500 // Speed of the tilt (0.5 seconds)
                // Use Easing.InOutQuad for a smoother start and stop at the edges
                easing.type: Easing.InOutQuad
                // Start where the previous loop ended (or at 0 on the first run)
                from: -5
                to: 5
            }

            RotationAnimation {
                duration: 1500 // Must match the first duration for symmetry
                easing.type: Easing.InOutQuad
                // Starts where the previous animation ended (20 degrees)
                from: 5
                to: -5
            }

            onRunningChanged: {
                if (!running) {
                    wave.rotation = 0;
                }
            }
        }
        Behavior on width {
            NumberAnimation {
                duration: card.isLoading ? card.brewingTime : 1000
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
                duration: card.isLoading ? card.brewingTime : 1000
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
            text: isLoading ? " Brewing..." : " €1.20 "
        }

        CustomButton {
            id: makeButton

            anchors.bottom: parent.bottom
            color: Theme.dark700
            text: !EdgeCoffeeMachine.isMakingDrink ? ("Make " + (targetBeverage ? targetBeverage.name : "")) : "Cancel"
            width: parent.width

            onClick: {
                console.log("Make drink button clicked");
                if (!EdgeCoffeeMachine.isMakingDrink) {
                    EdgeCoffeeMachine.makeDrink(targetBeverage);
                } else {
                    EdgeCoffeeMachine.stopBrewing();
                }
            }
        }
    }
}
