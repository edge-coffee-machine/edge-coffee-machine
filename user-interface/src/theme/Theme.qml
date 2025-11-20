pragma Singleton
import QtQuick 2.15

QtObject {
    id: appTheme

    // Colors
    property color dark700: "#161818"
    property color dark600: "#18191A"
    property color dark500: "#1A1B1C"
    property color dark400: "#212223"
    property color accent: "#D9A569"
    property color white: "#FFFFFF"

    property color textPrimary: "white"
    property color textSecondary: "#D9A569"

    // Layout
    property int spacing: 20
    property int h1: 32
}
