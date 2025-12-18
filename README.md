# ☕️ Edge Coffee Machine

## 🌍 Project Overview

Our client, the **Qt Group**, wants to showcase the capabilities of their frameworks in a **coffee machine** that uses Edge AI.

Coffee drinkers repeat the **same drink selection** every time, facing **slow** and **unintuitive** interfaces.

We’ll provide a **Qt for MCU**–powered UI integrated with **facial and voice recognition** that:

- Recognizes the user and prominently shows them drinks they might be interested in based on their selection history.
- Enables hands-free interaction through voice commands.

## 🏗️ Architecture

### Qt Target & Framework

This project uses **Qt for MCUs** (Qul), a lightweight Qt framework optimized for microcontroller-based embedded systems with limited resources.

- **UI Framework**: Qt for MCUs (`Qul::Core`, `Qul::Controls`)
- **QML Engine**: MCU-optimized QML with restricted feature set
- **Build System**: CMake with `qul_add_target()` for MCU project generation

### Supported Toolchains & Targets

The project supports the following build configurations:

- **Embedded MCU Target**:
  - **Toolchain**: GCC/Clang
  - **Board**: ESP32-P4-Function-EV-Board
  - **Build Type**: Debug/Release

The primary deployment target is embedded MCU hardware. Desktop previews are available within Qt Creator for development convenience.

### C++ Domain Types & QML Integration

**Domain Layer** ([`qmlproject/logic/`](./qmlproject/logic/)):

- **Beverage**: Represents a drink configuration with customizable ingredient parameters (coffee, milk, water, etc.)
- **User**: Represents a user profile that:
  - Tracks beverage **weights** (preference scores) for each drink based on selection history
  - Maintains behavior scores (`tryerScore`, `customizerScore`) to classify users into three categories:
    - **Default** (new users)
    - **Conservative** (stick to favorites)
    - **Early Adopter** (frequent experimenters)
  - Categorization determines UI behavior (e.g., Early Adopters see variety suggestions)
- **EdgeCoffeeMachine**: Central singleton controller managing machine state, user sessions, and brewing simulation
- **RecipeDatabase**: Manages persistent drink recipe data
- **BeverageListModel** / **UserListModel**: Qt-compatible data models bridging C++ domain objects to QML

**QML Integration**:

- Domain types inherit from `Qul::Object` and expose properties via `Qul::Property<T>`
- Models (e.g., `BeverageListModel`) adapt domain collections to Qt's model-view pattern for QML list views
- `EdgeCoffeeMachine` is registered as a `Qul::Singleton`, making it globally accessible in QML as `EdgeCoffeeMachine`
- Data binding synchronizes UI state with backend logic in real-time

### Project Structure

```
qmlproject/
├── main.qml                    # Application entry point
├── logic/                      # C++ domain layer
│   ├── EdgeCoffeeMachine.{h,cpp}   # Core singleton controller
│   ├── Beverage.{h,cpp}            # Drink model
│   ├── User.{h,cpp}                # User profile model
│   ├── RecipeDatabase.{h,cpp}      # Recipe persistence
│   └── *ListModel.h                # QML-compatible data models
└── user-interface/
    └── src/
        ├── components/         # Reusable QML UI components
        └── views/              # Full-screen QML views
```

## 🚀 Development Rules

### 🧹 Formatting

#### QML Files

Use `qmlformat` on all QML files before committing.
The [configuration](https://doc.qt.io/qt-6/qtqml-tooling-qmlformat.html#settings-file) file is located at the project root: [`.qmlformat.ini`](./.qmlformat.ini).

Qt Creator can be set up to format on save:

1. Go to **Settings > Qt Quick > Code Style**
   1. Click on **Copy...** to create a custom style (name it as you like)
   1. Under **Formatter Selection**, select **Custom Formatter** (ensure `qmlformat` is installed on your system)[^1]
   1. Under **Custom Formatter Configuration**, select the path to the `qmlformat` executable (e.g., `/opt/homebrew/bin/qmlformat`)
1. Go to **Settings > Qt Quick > QML/JS Editing**
   1. Under **Formatting**, check **Enable auto format on file save**

## 📘 References

- Frontend uses **[Qt for MCUs](https://doc.qt.io/QtForMCUs/)**

[^1]: Select this option and not **QmlFormat** since the latter doesn't format on save correctly.
