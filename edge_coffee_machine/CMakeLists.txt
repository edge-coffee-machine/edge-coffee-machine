cmake_minimum_required(VERSION 3.20)

# Include the standard ESP-IDF project helper
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Add link option required by Qt for MCUs
idf_build_set_property(LINK_OPTIONS "-Wno-psabi" APPEND)

# Disable LVGL graphics in ESP-WHO components (since we use Qt)
add_compile_definitions(BSP_CONFIG_NO_GRAPHIC_LIB=1)


# Define the project
project(edge_coffee_machine)
