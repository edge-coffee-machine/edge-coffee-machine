import QtQuick
import QtQuick.Controls

import Theme
import "../text"

Item {
    id: sliderInputRoot

    property alias label: labelText.text // Expose the text property for external setting

    property double maximumValue: 100.0
    property double minimumValue: 0.0
    property string unitOfM: "ml"

    // Public properties for the slider
    property double value: 0.5
    property double initialValue: 0.1
    onInitialValueChanged: {
        actualSlider.value = initialValue;
    }

    function getIcon(label: string): string {
        if (label === "Coffee" || label === "Powder") {
            return "assets/icons/coffee.png";
        }

        if (label === "Foam") {
            return "assets/icons/foam.png";
        }

        if (label === "Water") {
            return "assets/icons/water.png";
        }

        if (label === "Milk") {
            return "assets/icons/milk.png";
        }

        return "assets/icons/coffee.png";
    }

    height: 40
    width: 240

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: icon

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
            height: 25
            source: sliderInputRoot.getIcon(sliderInputRoot.label)
            width: 25
        }

        Item {
            anchors {
                left: icon.right
                leftMargin: 5
            }

            TextDefault {
                id: labelText

                color: "white"
                font.weight: 700
                font.pixelSize: 12
                y: -13
            }

            TextDefault {
                id: quantity

                color: "white"
                font.weight: 400
                font.pixelSize: 12
                opacity: 0.6
                text: actualSlider.value.toFixed(1) + " " + sliderInputRoot.unitOfM + "-" + sliderInputRoot.value// Display label and current value

                anchors {
                    top: labelText.bottom
                }
            }
        }
    }

    Slider {
        id: actualSlider

        visible: sliderInputRoot.minimumValue !== sliderInputRoot.maximumValue

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        from: sliderInputRoot.minimumValue
        stepSize: 1
        to: sliderInputRoot.maximumValue
        value: sliderInputRoot.initialValue
        width: 150

        // Optional: Customize appearance to match original Rectangle if desired
        background: Rectangle {
            color: Theme.dark600
            height: implicitHeight
            implicitHeight: 12
            implicitWidth: 200
            radius: height / 2
            width: actualSlider.availableWidth
            x: actualSlider.leftPadding
            y: actualSlider.topPadding + actualSlider.availableHeight / 2 - height / 2

            Rectangle {
                color: Theme.white
                height: parent.height
                radius: height / 2
                width: actualSlider.visualPosition * (parent.width - handle.width) + handle.width
            }
        }
        handle: Rectangle {
            id: handle

            color: "white"
            implicitHeight: 12
            implicitWidth: 12
            radius: height / 2
            x: actualSlider.leftPadding + actualSlider.visualPosition * (actualSlider.availableWidth - width)
            y: actualSlider.topPadding + actualSlider.availableHeight / 2 - height / 2
        }

        // Bind the slider's value back to the root item's value property
        onValueChanged: sliderInputRoot.value = actualSlider.value
    }

    // Separator line at the bottom
    Rectangle {
        anchors.bottom: parent.bottom
        height: 1.2
        opacity: 0.05
        radius: 2
        width: parent.width
    }
}
