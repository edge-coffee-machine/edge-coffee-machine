idf_component_register(
    SRCS "main.cpp" "speech_commands_action.cpp"
    INCLUDE_DIRS "." "include"
    REQUIRES esp-sr
    PRIV_REQUIRES freertos hardware_driver logic esp_codec_dev esp32_p4_function_ev_board espressif__esp-sr
)
# Ovo nalaže linkeru da ne ignoriše simbole iz ove komponente
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_speech_recognition")
