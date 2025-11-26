import QtQuick
import "../components/text"
import "../components"
import "../theme"

Rectangle {
    id: root

    property int p_spacing: 20

    // Signal to request going back to previous view
    signal goBackRequested

    color: Theme.dark500
    height: 600
    width: 1024

    // --- 1. HEADER SECTION ---
    Rectangle {
        anchors.fill: parent
        anchors.margins: 30
        color: "transparent"

        // --- 2. CLOSE BUTTON ---
        CloseButton {
            onGoBackRequested: root.goBackRequested()
        }

        Rectangle {
            anchors.centerIn: parent
            color: "transparent"
            height: 280
            width: parent.width - 100

            Column {
                id: headerContainer

                anchors.top: parent.top
                spacing: 2
                x: left_fader.width

                H1 {
                    text: "Good Morning,"
                }

                Text {
                    color: "#AAAAAA"
                    font.pixelSize: 28
                    font.weight: 300
                    text: "Who’s having a coffee?"
                }
            }

            // --- 3. CONTENT AREA (Holds List + Fixed Button) ---
            Item {
                id: contentArea

                // Position: Below header, fixed height
                anchors.top: headerContainer.bottom
                anchors.topMargin: 30
                height: 200

                // Width: Screen width minus margins (50px left, 50px right)
                width: parent.width

                // A. THE FIXED NEW USER BUTTON (Right Aligned)
                Item {
                    id: fixedNewUser

                    anchors.right: parent.right
                    anchors.rightMargin: left_fader.width // Ensure centering
                    anchors.top: parent.top
                    height: 200
                    width: 100
                    z: 10 // Ensure it sits above scrolling content if overlap occurs

                    // Draw a rectangle (don't use text as it can't be centered properly)
                    Rectangle {
                        id: newUserBubble

                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        color: "#EFA968" // The Orange Color
                        height: 100
                        radius: 50
                        width: 100

                        // 2. The Geometric Plus (Perfectly Centered White Lines)
                        Rectangle {
                            anchors.centerIn: parent
                            color: "#FFFFFF"
                            height: 5
                            radius: 2
                            width: 36
                        }

                        Rectangle {
                            anchors.centerIn: parent
                            color: "#FFFFFF"
                            height: 36
                            radius: 2
                            width: 5
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: newUserBubble.bottom
                        anchors.topMargin: 15
                        color: "#FFFFFF"
                        font.bold: true
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 1.1
                        text: "New\nUser"
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            console.log("Create User");
                            console.log(edgeCoffeeMachineController.users);
                        }
                    }
                }

                Image {
                    id: fader

                    anchors.right: fixedNewUser.left
                    anchors.rightMargin: root.p_spacing
                    height: 200
                    source: "../../assets/img/fade-out.png"
                    width: 50
                    z: 1
                }

                Image {
                    id: left_fader

                    anchors.left: userList.left
                    height: 200
                    scale: -1
                    source: "../../assets/img/fade-out.png"
                    width: 25
                    z: 1
                }

                // B. THE SCROLLABLE LIST (Left Aligned, fills space up to New User)
                ListView {
                    id: userList

                    property real contentRealWidth: count * (itemWidth + spacing) - spacing

                    // Centering Logic (Applies only within the list area)
                    // If users fit in the space, center them. If not, scroll.
                    property real itemWidth: 100

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left

                    // Anchor right side to the New User button with spacing
                    anchors.right: fixedNewUser.left
                    anchors.rightMargin: 20
                    anchors.top: parent.top
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true // Clips items when they scroll towards the New User button

                    delegate: userDelegate
                    flickableDirection: Flickable.HorizontalFlick
                    model: edgeCoffeeMachineController.users
                    orientation: ListView.Horizontal
                    spacing: root.p_spacing

                    footer: Item {
                        height: 0
                        // This Item acts as the right-side padding/spacer.
                        width: fader.width
                    }
                    header: Item {
                        height: 0
                        // This Item acts as the right-side padding/spacer.
                        width: left_fader.width
                    }
                }
            }

            // --- 5. DELEGATE (Users Only) ---
            Component {
                id: userDelegate

                Item {
                    height: 200
                    width: 100

                    Rectangle {
                        id: avatarBubble

                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        border.color: "#444444"
                        border.width: 2
                        color: "#252525"
                        height: 100
                        radius: 50
                        width: 100

                        Text {
                            anchors.centerIn: parent
                            color: "#FFFFFF"
                            font.bold: true
                            font.pixelSize: 42
                            // Auto Initials
                            text: name.charAt(0).toUpperCase() + name.charAt(1).toUpperCase()
                        }
                    }

                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: avatarBubble.bottom
                        anchors.topMargin: 15
                        spacing: 4
                        width: parent.width

                        Text {
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                            font.bold: true
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            text: model.name
                            width: parent.width
                        }
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            edgeCoffeeMachineController.setUser(modelData);
                            console.log("Selected", model.name);
                            root.goBackRequested();
                        }
                    }
                }
            }
        }
    }
}
