pragma Singleton
import QtQuick 2.2

QtObject {
    property bool darkTheme: true // State variable to switch themes
    property color backgroundColor: darkTheme ? "#333333" : "white"
    property color primaryTextColor: darkTheme ? "white" : "black"

    // Function to change the theme
    function setDarkTheme(isDark) {
        darkTheme = isDark
    }
}
