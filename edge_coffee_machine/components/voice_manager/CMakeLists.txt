idf_component_register(
    SRCS "VoiceEngine.cpp"
    INCLUDE_DIRS "." "include"
    REQUIRES esp-sr esp_codec_dev esp32_p4_function_ev_board
    PRIV_REQUIRES freertos hardware_driver logic
)
# Ovo nalaže linkeru da ne ignoriše simbole iz ove komponente
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_speech_recognition")
