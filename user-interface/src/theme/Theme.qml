pragma Singleton

import QtQuick

QtObject {
    id: appTheme

    property color accent: "#D9A569"
    property color dark400: "#212223"
    property color dark500: "#1A1B1C"
    property color dark600: "#18191A"

    // Colors
    property color dark700: "#161818"
    property int h1: 32

    // Layout
    property int spacing: 20
    property color textPrimary: "white"
    property color textSecondary: "#D9A569"
    property color white: "#FFFFFF"
}
