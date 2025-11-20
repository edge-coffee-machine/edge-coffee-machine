import QtQuick 2.15
import QtQuick.Controls 2.15

import "../../theme"
import "../text"

Item {
    id: sliderInputRoot
    width: 240
    height: 40

    // Public properties for the slider
    property double value: 0.5
    property double minimumValue: 0.0
    property double maximumValue: 1.0
    property alias label: labelText.text // Expose the text property for external setting

    TextDefault {
        id: labelText
        text: "Label: " + sliderInputRoot.value.toFixed(2) // Display label and current value
        color: "white"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - actualSlider.width - 10 // Adjust width to make space for slider
    }

    Slider {
        id: actualSlider
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        from: sliderInputRoot.minimumValue
        to: sliderInputRoot.maximumValue
        value: sliderInputRoot.value
        width: 150

        // Bind the slider's value back to the root item's value property
        onValueChanged: sliderInputRoot.value = actualSlider.value

        // Optional: Customize appearance to match original Rectangle if desired
        background: Rectangle {
            radius: height / 2
            color: Theme.dark600

            Rectangle {
                width: actualSlider.visualPosition * (parent.width - handle.width) + handle.width
                height: parent.height
                color: Theme.white
                radius: height / 2
            }
        }
        handle: Rectangle {
            id: handle
            x: actualSlider.visualPosition * (actualSlider.width - width)
            implicitWidth: 12
            implicitHeight: 12
            radius: height / 2
            color: "white"
        }
    }

    // Separator line at the bottom
    Rectangle {
        width: parent.width
        height: 1.2
        opacity: 0.05
        radius: 2
        anchors.bottom: parent.bottom
    }
}
