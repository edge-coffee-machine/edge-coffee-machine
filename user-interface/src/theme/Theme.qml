pragma Singleton
import QtQuick

QtObject {
    id: appTheme

    // UI colors
    readonly property color accent: "#D9A569"
    readonly property color dark400: "#212223"
    readonly property color dark500: "#1A1B1C"
    readonly property color dark600: "#18191A"
    readonly property color dark700: "#161818"

    // Font sizes
    readonly property int h1: 32

    // Layout
    readonly property int spacing: 20

    // Text colors
    readonly property color textPrimary: "white"
    readonly property color textSecondary: "#D9A569"
    readonly property color white: "#FFFFFF"
}
