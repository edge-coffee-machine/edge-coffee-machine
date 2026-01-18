idf_component_register(SRCS "FaceRecognitionApp.cpp" "frame_cap_pipeline.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES who_recognition_app who_frame_cap
                       PRIV_REQUIRES logic who_cam who_spiflash_fatfs)