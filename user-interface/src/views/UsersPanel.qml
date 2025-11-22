import QtQuick 2.15
import "../theme"
import "../components/text"
import "../components"

Rectangle {
    id: root
    width: 1024
    height: 600
    color: Theme.dark500

    // Signal to request going back to previous view
    signal goBackRequested
    property int p_spacing: 20

    // --- 1. HEADER SECTION ---
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        anchors.margins: 30

        // --- 2. CLOSE BUTTON ---
        CloseButton {
            onGoBackRequested: root.goBackRequested()
        }

        Rectangle {
            width: parent.width - 100
            height: 280
            anchors.centerIn: parent
            color: "transparent"

            Column {
                id: headerContainer
                anchors.top: parent.top
                x: left_fader.width

                spacing: 2

                H1 {
                    text: "Good Morning,"
                }

                Text {
                    text: "Who’s having a coffee?"
                    color: "#AAAAAA"
                    font.pixelSize: 28
                    font.weight: 300
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
                    width: 100
                    height: 200
                    anchors.right: parent.right
                    anchors.rightMargin: left_fader.width // Ensure centering
                    anchors.top: parent.top
                    z: 10 // Ensure it sits above scrolling content if overlap occurs

                    // Draw a rectangle (don't use text as it can't be centered properly)
                    Rectangle {
                        id: newUserBubble
                        width: 100
                        height: 100
                        radius: 50
                        color: "#EFA968" // The Orange Color
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter

                        // 2. The Geometric Plus (Perfectly Centered White Lines)
                        Rectangle {
                            width: 36
                            height: 5
                            color: "#FFFFFF"
                            radius: 2
                            anchors.centerIn: parent
                        }
                        Rectangle {
                            width: 5
                            height: 36
                            color: "#FFFFFF"
                            radius: 2
                            anchors.centerIn: parent
                        }
                    }

                    Text {
                        anchors.top: newUserBubble.bottom
                        anchors.topMargin: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "New\nUser"
                        color: "#FFFFFF"
                        font.pixelSize: 16
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 1.1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: console.log("Create User")
                    }
                }

                Image {
                    id: fader
                    source: "../../assets/img/fade-out.png"
                    width: 50
                    height: 200
                    anchors.right: fixedNewUser.left
                    anchors.rightMargin: p_spacing
                    z: 1
                }

                Image {
                    id: left_fader
                    source: "../../assets/img/fade-out.png"
                    width: 25
                    height: 200
                    anchors.left: userList.left
                    z: 1
                    scale: -1
                }
                // B. THE SCROLLABLE LIST (Left Aligned, fills space up to New User)
                ListView {
                    id: userList
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    // Anchor right side to the New User button with spacing
                    anchors.right: fixedNewUser.left
                    anchors.rightMargin: 20

                    clip: true // Clips items when they scroll towards the New User button

                    orientation: ListView.Horizontal
                    spacing: p_spacing

                    // Centering Logic (Applies only within the list area)
                    // If users fit in the space, center them. If not, scroll.
                    property real itemWidth: 100
                    property real contentRealWidth: count * (itemWidth + spacing) - spacing

                    model: userModel
                    delegate: userDelegate

                    flickableDirection: Flickable.HorizontalFlick
                    boundsBehavior: Flickable.StopAtBounds

                    header: Item {
                        // This Item acts as the right-side padding/spacer.
                        width: left_fader.width
                        height: 0
                    }

                    footer: Item {
                        // This Item acts as the right-side padding/spacer.
                        width: fader.width
                        height: 0
                    }
                }
            }

            // --- 4. DATA MODEL (Real Users Only) ---
            ListModel {
                id: userModel
                // Guest Removed, New User Removed (it's now a fixed item)
                ListElement {
                    firstName: "Matteo"
                    lastName: "Rossi"
                }
                ListElement {
                    firstName: "Sophie"
                    lastName: "Carter"
                }
                ListElement {
                    firstName: "Lucas"
                    lastName: "Bennet"
                }
                ListElement {
                    firstName: "Anna"
                    lastName: "Lee"
                }
                ListElement {
                    firstName: "John"
                    lastName: "Doe"
                }
                ListElement {
                    firstName: "Elena"
                    lastName: "Ricci"
                }
                ListElement {
                    firstName: "John"
                    lastName: "White"
                }
            }

            // --- 5. DELEGATE (Users Only) ---
            Component {
                id: userDelegate

                Item {
                    width: 100
                    height: 200

                    Rectangle {
                        id: avatarBubble
                        width: 100
                        height: 100
                        radius: 50
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter

                        color: "#252525"
                        border.width: 2
                        border.color: "#444444"

                        Text {
                            anchors.centerIn: parent
                            // Auto Initials
                            text: firstName.charAt(
                                      0) + (lastName ? lastName.charAt(0) : "")
                            font.pixelSize: 42
                            font.bold: true
                            color: "#FFFFFF"
                        }
                    }

                    Column {
                        anchors.top: avatarBubble.bottom
                        anchors.topMargin: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width
                        spacing: 4

                        Text {
                            text: firstName
                            color: "#FFFFFF"
                            font.pixelSize: 18
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
                            elide: Text.ElideRight
                        }

                        Text {
                            visible: lastName !== ""
                            text: lastName
                            color: "#AAAAAA"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
                            elide: Text.ElideRight
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: console.log("Selected", firstName)
                    }
                }
            }
        }
    }
}
