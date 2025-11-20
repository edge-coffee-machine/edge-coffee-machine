import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 800
    height: 480
    color: "#050505"

    // Signal to request going back to previous view
    signal goBackRequested()

    // --- 1. HEADER SECTION ---
    Column {
        id: headerContainer
        anchors.top: parent.top
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 8

        Text {
            text: "Good Morning,"
            color: "#FFFFFF"
            font.pixelSize: 34
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: "Who’s having a coffee?"
            color: "#AAAAAA"
            font.pixelSize: 22
            font.weight: Font.Light
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // --- 2. CLOSE BUTTON ---
    Rectangle {
        width: 40; height: 40; radius: 20
        color: "#333333"
        anchors { top: parent.top; right: parent.right; margins: 20 }
        Text { anchors.centerIn: parent; text: "✕"; color: "white"; font.pixelSize: 16 }
        MouseArea { anchors.fill: parent; onClicked: root.goBackRequested() }
    }

    // --- 3. CONTENT AREA (Holds List + Fixed Button) ---
    Item {
        id: contentArea
        // Position: Below header, fixed height
        anchors.top: headerContainer.bottom
        anchors.topMargin: 60
        anchors.horizontalCenter: parent.horizontalCenter
        height: 200

        // Width: Screen width minus margins (50px left, 50px right)
        width: parent.width - 100

        // A. THE FIXED NEW USER BUTTON (Right Aligned)
        Item {
            id: fixedNewUser
            width: 100
            height: 200
            anchors.right: parent.right
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
                    width: 36; height: 5
                    color: "#FFFFFF"; radius: 2
                    anchors.centerIn: parent
                }
                Rectangle {
                    width: 5; height: 36
                    color: "#FFFFFF"; radius: 2
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

            MouseArea { anchors.fill: parent; onClicked: console.log("Create User") }
        }

        // B. THE SCROLLABLE LIST (Left Aligned, fills space up to New User)
        ListView {
            id: userList
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            // Anchor right side to the New User button with spacing
            anchors.right: fixedNewUser.left
            anchors.rightMargin: 30

            clip: true // Clips items when they scroll towards the New User button

            orientation: ListView.Horizontal
            spacing: 30

            // Centering Logic (Applies only within the list area)
            // If users fit in the space, center them. If not, scroll.
            property real itemWidth: 100
            property real contentRealWidth: count * (itemWidth + spacing) - spacing
            leftMargin: (width > contentRealWidth) ? (width - contentRealWidth) / 2 : 0

            model: userModel
            delegate: userDelegate

            flickableDirection: Flickable.HorizontalFlick
            boundsBehavior: Flickable.StopAtBounds
        }
    }

    // --- 4. DATA MODEL (Real Users Only) ---
    ListModel {
        id: userModel
        // Guest Removed, New User Removed (it's now a fixed item)
        ListElement { firstName: "Matteo"; lastName: "Rossi" }
        ListElement { firstName: "Sophie"; lastName: "Carter" }
        ListElement { firstName: "Lucas"; lastName: "Bennet" }
        ListElement { firstName: "Anna"; lastName: "Lee" }
        ListElement { firstName: "John"; lastName: "Doe" }
        ListElement { firstName: "Elena"; lastName: "Ricci" }
    }

    // --- 5. DELEGATE (Users Only) ---
    Component {
        id: userDelegate

        Item {
            width: 100
            height: 200

            Rectangle {
                id: avatarBubble
                width: 100; height: 100; radius: 50
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter

                color: "#252525"
                border.width: 2; border.color: "#444444"

                Text {
                    anchors.centerIn: parent
                    // Auto Initials
                    text: firstName.charAt(0) + (lastName ? lastName.charAt(0) : "")
                    font.pixelSize: 42
                    font.bold: true
                    color: "#FFFFFF"
                }
            }

            Column {
                anchors.top: avatarBubble.bottom; anchors.topMargin: 15
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                spacing: 4

                Text {
                    text: firstName
                    color: "#FFFFFF"
                    font.pixelSize: 18; font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width; elide: Text.ElideRight
                }

                Text {
                    visible: lastName !== ""
                    text: lastName
                    color: "#AAAAAA"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width; elide: Text.ElideRight
                }
            }

            MouseArea { anchors.fill: parent; onClicked: console.log("Selected", firstName) }
        }
    }
}
