import QtQuick
import QtQuick.Controls

import "../../theme"
import "../text"

Item {
    id: sliderInputRoot
    width: 240
    height: 40

    // Public properties for the slider
    property double value: 0.5
    property double minimumValue: 0.0
    property double maximumValue: 100.0
    property string unitOfM: "ml"
    property alias label: labelText.text // Expose the text property for external setting

    function getIcon(label) {
        if (label === "Coffee" || label === "Powder") {
            return "../../../assets/icons/coffee.png"
        }

        if (label === "Foam") {
            return "../../../assets/icons/foam.png"
        }

        if (label === "Water") {
            return "../../../assets/icons/water.png"
        }

        if (label === "Milk") {
            return "../../../assets/icons/milk.png"
        }

        return "../../../assets/icons/coffee.png"
    }

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        Image {
            id: icon
            source: sliderInputRoot.getIcon(sliderInputRoot.label)
            width: 25
            height: 25
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            anchors {
                left: icon.right
                leftMargin: 5
            }

            TextDefault {
                id: labelText
                y: -13
                color: "white"
                font.weight: 600
            }

            TextDefault {
                id: quantity
                text: sliderInputRoot.value.toFixed(
                          1) + " " + sliderInputRoot.unitOfM // Display label and current value
                color: "white"
                anchors {
                    top: labelText.bottom
                }
                opacity: 0.6
                font.weight: 400
            }
        }
    }

    Slider {
        id: actualSlider
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        from: sliderInputRoot.minimumValue
        to: sliderInputRoot.maximumValue
        value: sliderInputRoot.value
        width: 150
        stepSize: 10

        // Bind the slider's value back to the root item's value property
        onValueChanged: sliderInputRoot.value = actualSlider.value

        // Optional: Customize appearance to match original Rectangle if desired
        background: Rectangle {
            x: actualSlider.leftPadding
            y: actualSlider.topPadding + actualSlider.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: 12
            width: actualSlider.availableWidth
            height: implicitHeight
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
            x: actualSlider.leftPadding + actualSlider.visualPosition
               * (actualSlider.availableWidth - width)
            y: actualSlider.topPadding + actualSlider.availableHeight / 2 - height / 2
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
