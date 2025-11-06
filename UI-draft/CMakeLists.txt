cmake_minimum_required(VERSION 3.16)

project(EdgeCoffeeMachine_Quick VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appEdgeCoffeeMachine_Quick
    main.cpp
    resources.qrc
)

qt_add_qml_module(appEdgeCoffeeMachine_Quick
    URI EdgeCoffeeMachine_Quick
    QML_FILES
        Main.qml
        QML_FILES LightButton.qml
        QML_FILES test.qml
        QML_FILES Theme.qml
        QML_FILES components/BottomAssistant.qml
        QML_FILES components/DrinkListPanel.qml
        QML_FILES components/FavouritePanel.qml
        QML_FILES components/Header.qml
        QML_FILES components/SettingsPanel.qml
        QML_FILES components/Card.qml
        QML_FILES components/Theme.qml
        QML_FILES components/CustomButton.qml
        RESOURCES resources.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appEdgeCoffeeMachine_Quick PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appEdgeCoffeeMachine_Quick
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appEdgeCoffeeMachine_Quick
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appEdgeCoffeeMachine_Quick
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
