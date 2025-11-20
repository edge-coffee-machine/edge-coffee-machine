import QtQuick
import "../theme"

Rectangle {
    id: card
    width: 300; height: 400
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
        width: parent.width
        height: 90
        anchors.bottom: progressFill.top
        anchors.bottomMargin: -50
    }

    Rectangle {
        id: progressFill
        // Connect width directly to the button's enabled state (inverted)
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
