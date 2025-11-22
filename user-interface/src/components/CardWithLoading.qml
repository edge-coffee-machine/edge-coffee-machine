import QtQuick
import "../theme"

Rectangle {
    id: card
    width: 300
    height: 400
    radius: 16
    color: Theme.dark700
    clip: true

    property bool isLoading: false

    // Mask over the laoding animation
    Image {
        source: "../../assets/img/border.png"
        width: parent.width
        height: parent.height
        z: 1
    }

    Image {
        source: "../../assets/img/wave.png"
        width: isLoading ? (parent.width + 200) : parent.width + 100
        height: 90
        anchors.bottom: progressFill.top
        anchors.bottomMargin: -50
        anchors.horizontalCenter: parent.horizontalCenter

        Behavior on width {
            NumberAnimation {
                duration: 3000
                easing.type: Easing.InOutQuad
            }
        }

        SequentialAnimation on rotation {
            // Run the animation automatically
            running: isLoading

            // Make it spin forever
            loops: Animation.Infinite

            RotationAnimation {
                // Start where the previous loop ended (or at 0 on the first run)
                from: -5
                to: 5
                duration: 500 // Speed of the tilt (0.5 seconds)
                // Use Easing.InOutQuad for a smoother start and stop at the edges
                easing.type: Easing.InOutQuad
            }

            RotationAnimation {
                // Starts where the previous animation ended (20 degrees)
                from: 5
                to: -5
                duration: 500 // Must match the first duration for symmetry
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: progressFill
        width: parent.width
        height: isLoading ? parent.width : 0
        radius: 16
        color: Theme.accent // Or a distinct color like green
        opacity: 1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -50

        Behavior on height {
            NumberAnimation {
                duration: 3000
                easing.type: Easing.InOutQuad
            }
        }
    }

    // Allow placing arbitrary children inside the card by making 'content' the
    // default property that maps to an internal Item's data.
    default property alias content: contentItem.data

    // Internal content holder — keeps children inside card padding
    Item {
        id: contentItem
        anchors.fill: parent
        anchors.margins: 15
    }
}
