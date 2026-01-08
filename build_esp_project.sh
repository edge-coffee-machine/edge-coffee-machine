#!/bin/bash

# --------------------------------------------------------------------------
# 1. Verify QUL_DIR is set
# --------------------------------------------------------------------------
if [ -z "$QUL_DIR" ]; then
    echo "Error: QUL_DIR is not set."
    echo "Please set it using: export QUL_DIR=/path/to/your/qt/for/mcus"
    exit 1
fi

# Configuration
TARGET_PROJECT="edge_coffee_machine"
TEMP_BUILD="temp_build"

# --------------------------------------------------------------------------
# 2. Run qmlprojectexporter
# --------------------------------------------------------------------------
echo "Running qmlprojectexporter..."

# Note: Using 'edge_coffe_machine' as specified (check for typo 'coffee' if this fails)
"$QUL_DIR/bin/qmlprojectexporter" qmlproject/edge_coffe_machine.qmlproject \
  --platform esp32-p4-func-idf \
  --project-type cmake \
  --toolchain gnu \
  --outdir "$TEMP_BUILD" \
  --boarddefaults "$QUL_DIR/platform/boards/espressif/esp32-p4-func-idf/cmake/BoardDefaults_16bpp_default.qmlprojectconfig" \
  --platform-metadata "$QUL_DIR/platform/boards/espressif/esp32-p4-func-idf/esp32p4-idf_16bpp_Linux_xtensagcc-metadata.json"

# Check if exporter command was successful
if [ $? -ne 0 ]; then
    echo "Error: qmlprojectexporter failed. Aborting."
    exit 1
fi

# --------------------------------------------------------------------------
# 3. Move folders to project
# --------------------------------------------------------------------------
echo "Moving generated files to $TARGET_PROJECT..."

# Ensure the target directory exists
if [ ! -d "$TARGET_PROJECT" ]; then
    echo "Error: Target directory '$TARGET_PROJECT' does not exist."
    exit 1
fi

# Clean old folders in destination to prevent nesting (e.g. QtMCUs/QtMCUs)
rm -rf "$TARGET_PROJECT/CMake"
rm -rf "$TARGET_PROJECT/QtMCUs/generated"

# Move new folders
# Note: The tool output is 'QtMCUs', fixing casing from prompt 'QtMcus'
if [ -d "$TEMP_BUILD/CMake" ]; then
    mv "$TEMP_BUILD/CMake" "$TARGET_PROJECT/"
else
    echo "Warning: Generated CMake folder not found."
fi

if [ -d "$TEMP_BUILD/QtMCUs/generated" ]; then
    mv "$TEMP_BUILD/QtMCUs/generated" "$TARGET_PROJECT/"
else
    echo "Warning: Generated QtMCUs folder not found."
fi

# 4. Fix absolute paths (The requested Grep/Sed addition)
# --------------------------------------------------------------------------
echo "Fixing absolute paths in generated files..."

# We assume the exporter generated absolute paths containing 'temp_build'.
# This command recursively replaces 'temp_build' with 'edge_coffee_machine' 
# in all files inside QtMCUs and CMake folders.
# We use -print0 and -0 to handle filenames with spaces correctly.

find "$TARGET_PROJECT/QtMCUs" "$TARGET_PROJECT/CMake" -type f -print0 | xargs -0 sed -i "s|/$TEMP_BUILD/|/$TARGET_PROJECT/|g"

# Also try replacing relative occurrences just in case (e.g. without leading slash)
find "$TARGET_PROJECT/QtMCUs" "$TARGET_PROJECT/CMake" -type f -print0 | xargs -0 sed -i "s|$TEMP_BUILD|$TARGET_PROJECT|g"

# --------------------------------------------------------------------------
# 5. Cleanup and Build
# --------------------------------------------------------------------------
echo "Cleaning up temp files..."
rm -rf "$TEMP_BUILD"

echo "Building ESP-IDF project..."
cd "$TARGET_PROJECT" || exit

# Run the build
idf.py build