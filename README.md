# ☕️ Edge Coffee Machine

## 🌍 Project Overview

Our client, the **Qt Group**, wants to showcase the capabilities of their frameworks in a **coffee machine** that uses Edge AI.

Coffee drinkers repeat the **same drink selection** every time, facing **slow** and **unintuitive** interfaces.

We’ll provide a **Qt for MCU**–powered UI integrated with **facial and voice recognition** that:

- Recognizes the user and prominently shows them drinks they might be interested in based on their selection history.
- Enables hands-free interaction through voice commands.

## 🚀 Development Rules

### 🧹 Formatting

#### QML Files

Use `qmlformat` on all QML files before committing.
The configuration file is located at the project root: `.qmlformat.conf`.

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
