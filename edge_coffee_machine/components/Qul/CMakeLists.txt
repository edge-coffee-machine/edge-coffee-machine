idf_component_get_property(freertos_include_dir freertos ORIG_INCLUDE_PATH)
idf_component_register(SRCS dummy.c qul_run.cpp
    INCLUDE_DIRS ${freertos_include_dir}
    INCLUDE_DIRS "."
    PRIV_REQUIRES freertos esp_lcd esp_lcd_touch esp_timer esp_driver_ppa esp_mm esp32_p4_function_ev_board logic
)
set(QUL_DISABLE_REEXPORT ON)
find_package(QulExport REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/../.." NO_DEFAULT_PATH)
set(QulCoreLib ${QulExport_GENERIC_LIBRARIES})
list(FILTER QulCoreLib INCLUDE REGEX ".*libQulCore.*\.a")
add_prebuilt_library(QulCore ${QulCoreLib} PRIV_REQUIRES Qul)

target_sources(${COMPONENT_LIB} PRIVATE ${QulExport_SOURCES})
target_include_directories(${COMPONENT_LIB} PUBLIC ${QulExport_INCLUDES})
target_compile_definitions(${COMPONENT_LIB} PUBLIC ${QulExport_DEFINITIONS})
target_compile_options(${COMPONENT_LIB} PRIVATE ${QulExport_COMPILE_OPTIONS} "-Wa,-I${CMAKE_CURRENT_LIST_DIR}/../../QtMCUs/generated")
target_link_options(${COMPONENT_LIB} PRIVATE ${QulExport_LINK_OPTIONS})
target_link_libraries(${COMPONENT_LIB} PUBLIC "-Wl,--start-group" ${QulExport_LIBRARIES} QulCore "-Wl,--end-group")

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 14
    CXX_EXTENSIONS ON
    C_EXTENSIONS ON
)

if(CONFIG_ENABLE_PERFORMANCE_LOGGING)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC QUL_ENABLE_PERFORMANCE_LOGGING)
endif()
