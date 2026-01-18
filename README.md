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
- **Build Pipeline**:
  1.  **Source**: The [`qmlproject/`](./qmlproject/) directory contains the QML UI and C++ domain logic.
  2.  **Transpilation**: The [`build_esp_project.sh`](./build_esp_project.sh) script invokes `qmlprojectexporter`.
  3.  **Generation**: QML assets are transpiled into optimized C++ source code located in `edge_coffee_machine/QtMCUs/`.
  4.  **Linking**: The ESP-IDF project links these generated files via the `components/Qul` component.

### Domain Logic (`qmlproject/logic/`)

- **Beverage**: Drink configuration with customizable ingredients (coffee, milk, water).
- **User**: Profile tracking preference scores and behavior categories (*Default, Conservative, Early Adopter*).
- **EdgeCoffeeMachine**: Singleton controller managing state, sessions, and brewing simulation.
- **RecipeDatabase**: Manages persistent drink recipes.
- **Models**: `BeverageListModel` and `UserListModel` bridge C++ data to QML views.

### Project Structure

```
edge_coffee_machine/            # ESP-IDF project root
├── main/                       # Application entry point
├── components/                 # ESP-IDF components
│   ├── logic/                  # C++ domain logic
│   ├── voice_manager/          # Voice recognition logic
│   └── face_manager/           # Face recognition logic
├── QtMCUs/                     # Generated Qt for MCUs files
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

## 🛠️ Build Project: Embedded Target (ESP32-P4)

To manually build the project for the ESP32-P4 target, follow these steps. This process assumes you have the ESP-IDF environment set up and the Qt for MCUs SDK installed.

### 1. Set Environment Variables

Export the `QUL_DIR` variable pointing to your Qt for MCUs installation directory:

```bash
export QUL_DIR=/path/to/your/Qul
```

### 2. Run the Build Script

Execute the provided build script, which handles:
1.  Running `qmlprojectexporter` to generate C++ code from QML.
    **Note:** The `qmlprojectexporter` command in `build_esp_project.sh` uses Linux-specific options (`-platform esp32-p4-func-idf`, `-toolchain gnu`, and paths). If you are not using Linux, you may need to modify the script to match your operating system and toolchain.
2.  Moving generated files to the `edge_coffee_machine` project.
3.  Fixing include paths.
4.  Triggering the ESP-IDF build (`idf.py build`).

```bash
./build_esp_project.sh
```

**Note:** Ensure the script has execution permissions (`chmod +x build_esp_project.sh`).

### ⚡ Flash Binary

After building, you can flash the binary to your ESP32-P4 board using `idf.py flash`.

```bash
idf.py flash
```

**Important:** If you are re-flashing and want to clear any previously saved face recognition features, it is recommended to erase the flash entirely before flashing the new binary:

```bash
idf.py erase-flash flash
```

To monitor the serial output and verify the status of voice and face recognition, along with other application logs, use:

```bash
idf.py monitor
```

You can exit the monitor by pressing `Ctrl+]`.

## 💻 Build: Desktop Target (Windows/Linux)

To run the application on your PC (for development or visual testing), you must build the **Desktop** configuration.

> **⚠️ Important Note:**
> The Desktop build runs **only the UI and C++ Domain Logic**.
> Because the AI components (Face & Voice Recognition) are heavily optimized for the ESP32-P4 hardware, they are **excluded** from this target. In the Desktop environment, these features are disabled or mocked to allow for rapid UI iteration without hardware.

1. Open `qmlproject/edge_coffee_machine.qmlproject` in **Qt Creator**.
2. Configure the project using the **Qt for MCUs Desktop Kit**.
3. Build the project (Release or Debug).
4. Verify that the executable is generated in your build directory.

## 🧪 Visual Testing Setup (Windows)

To ensure the UI renders correctly across changes, we use a Visual Regression Testing system based on **Python + Pytest + OpenCV**.

### 1. Prerequisites
- Python 3.10 or newer installed.
- The project compiled successfully (an `.exe` must exist in the build folder).

### 2. Setup Virtual Environment
Run the following commands in PowerShell from the project root:

```powershell
# 1. Create virtual environment (only once)
python -m venv venv

# 2. Activate environment
.\venv\Scripts\Activate

# 3. Install dependencies
pip install -r requirements.txt
```

### 3. Run
```powershell
pytest tests/ui_tests
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
