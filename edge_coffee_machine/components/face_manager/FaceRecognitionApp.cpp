#include "FaceRecognitionApp.h"
#include "EdgeCoffeeMachine.h"
#include <platforminterface/log.h>
#include <sstream>

#include "frame_cap_pipeline.hpp"
#include "who_spiflash_fatfs.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void recognition_task_entry(void *arg)
{
    FaceRecognitionApp *app = static_cast<FaceRecognitionApp *>(arg);
    app->run();
    vTaskDelete(NULL);
}

FaceRecognitionApp::FaceRecognitionApp(who::frame_cap::WhoFrameCap *frame_cap)
    : who::app::WhoRecognitionAppTerm(frame_cap)
{
    Qul::PlatformInterface::log("[FaceRecognitionApp] Initialized successfully.\n");
}

void FaceRecognitionApp::recognition_result_cb(const std::string &result)
{
    Qul::PlatformInterface::log("[FaceRecognitionApp] Result: %s\n", result.c_str());

    if (result.find("ID: ") != std::string::npos) {
        int id = -1;
        bool enrolling = false;
        
        std::stringstream ss(result);
        std::string part;
        
        while(std::getline(ss, part, ',')) {
            size_t id_pos = part.find("ID: ");
            if (id_pos != std::string::npos) {
                id = std::stoi(part.substr(id_pos + 4));
            }
            
            size_t enroll_pos = part.find("Enrolling: ");
            if (enroll_pos != std::string::npos) {
                if (part.find("yes") != std::string::npos) {
                    enrolling = true;
                }
            }
        }

        if (id != -1) {
            if (enrolling) {
                Qul::PlatformInterface::log("[FaceRecognitionApp] Enrolling user with ID: %d\n", id);
                Logic::EdgeCoffeeMachine::instance().enrollUser(id);
            } else {
                Qul::PlatformInterface::log("[FaceRecognitionApp] Identifying user with ID: %d\n", id);
                Logic::EdgeCoffeeMachine::instance().identifyUser(id);
            }
        }
    } else if (result.find("Unknown face") != std::string::npos) {
        Qul::PlatformInterface::log("[FaceRecognitionApp] Unknown face detected, logging out.\n");
        Logic::EdgeCoffeeMachine::instance().logoutUser();
    }
}

void start_face_recognition()
{
#if CONFIG_DB_FATFS_FLASH
    ESP_ERROR_CHECK(fatfs_flash_mount());
#elif CONFIG_DB_SPIFFS
    ESP_ERROR_CHECK(bsp_spiffs_mount());
#endif
#if CONFIG_DB_FATFS_SDCARD || CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

    who::frame_cap::WhoFrameCap *frame_cap = get_mipi_csi_frame_cap_pipeline();
    FaceRecognitionApp *app = new FaceRecognitionApp(frame_cap);

    xTaskCreate(recognition_task_entry, "recognition_task", 4096, app, 5, NULL);
}