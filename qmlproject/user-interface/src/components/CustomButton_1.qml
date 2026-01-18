import QtQuick 2.0
import QtQuick.Templates 2.15

Button {
    id: btn

    bottomPadding: 20
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    leftPadding: 28
    rightPadding: 28
    topPadding: 20

    background: Rectangle {
        color: "green"
        opacity: btn.down ? 0.8 : 1
        radius: 4
    }
    contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        text: btn.text
        verticalAlignment: Text.AlignVCenter
    }
}
