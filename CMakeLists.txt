cmake_minimum_required(VERSION 3.21) # Qt 6.2 requires CMake 3.21 or higher
project(EdgeCoffeeMachine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable QML Language Server configuration generation
set(QT_QML_GENERATE_QMLLS_INI ON)

# Find Qt6 packages
# If CMake struggles to find Qt6, you might need to set CMAKE_PREFIX_PATH or Qt6_DIR.
# For example, on Windows, if Qt is installed in C:\Qt\6.x.x\msvc2019_64, you might set:
# set(CMAKE_PREFIX_PATH "C:/Qt/6.x.x/msvc2019_64")
# or set the environment variable Qt6_DIR to point to the Qt6Config.cmake file.
find_package(Qt6 6.2 COMPONENTS Quick Qml Core Gui REQUIRED)

# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

# Add the executable for the coffee machine application
qt_add_executable(coffee_machine_app
    logic/main.cpp
	logic/EdgeCoffeeMachine.cpp
    logic/Beverage.cpp
    logic/User.cpp
)

# Set include directories for the C++ source files
target_include_directories(coffee_machine_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logic)

# Find all QML files recursively
file(GLOB_RECURSE QML_FILES "${CMAKE_CURRENT_SOURCE_DIR}/user-interface/*.qml")
file(GLOB_RECURSE QMLDIR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/user-interface/*/qmldir")

# Find all asset files recursively
file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_SOURCE_DIR}/user-interface/assets/*")

# Add all QML files and assets as resources, preserving the directory structure
qt_add_resources(coffee_machine_app "EdgeCoffeeMachine"
    PREFIX "/EdgeCoffeeMachine"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/user-interface"
    FILES ${QML_FILES} ${ASSET_FILES} ${QMLDIR_FILES}
)

# Link Qt libraries to the executable
target_link_libraries(coffee_machine_app
    PRIVATE
        Qt6::Quick
        Qt6::Qml
        Qt6::Core
        Qt6::Gui
)
