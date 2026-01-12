#include "FaceRecognitionApp.h"
#include "EdgeCoffeeMachine.h"
#include <platforminterface/log.h>
#include <sstream>

#include "frame_cap_pipeline.hpp"
#include "who_spiflash_fatfs.hpp"
#include "who_recognition.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void recognition_task_entry(void *arg)
{
    FaceRecognitionApp *app = static_cast<FaceRecognitionApp *>(arg);
    app->run();
    vTaskDelete(NULL);
}

FaceRecognitionApp::FaceRecognitionApp(who::frame_cap::WhoFrameCap *frame_cap)
    : who::app::WhoRecognitionAppTerm(frame_cap), m_is_waiting_for_face(false), m_frame_counter(0) 
{
    auto recognition_task = m_recognition->get_recognition_task();
    auto detect_task = m_recognition->get_detect_task();

    recognition_task->set_detect_result_cb(
        std::bind(&FaceRecognitionApp::detect_result_cb, this, std::placeholders::_1));

    detect_task->set_detect_result_cb(
        std::bind(&FaceRecognitionApp::detect_result_cb, this, std::placeholders::_1));

    Qul::PlatformInterface::log("[FaceRecognitionApp] Initialized for ESP32-P4.\n");
}

void FaceRecognitionApp::detect_result_cb(const who::detect::WhoDetect::result_t &result)
{
    m_frame_counter++;
    if (m_frame_counter % 60 == 0) { // Log every ~30 frames (approx 1 sec)
        Qul::PlatformInterface::log("[FaceRecognitionApp] Heartbeat: Frame %d. Waiting: %s. Faces visible: %d\n", 
                                    m_frame_counter, 
                                    m_is_waiting_for_face ? "YES" : "NO", 
                                    (int)result.det_res.size());
    }
    if (m_frame_counter % 300 == 0 && m_is_waiting_for_face) { // Log every ~30 frames (approx 1 sec)
        Logic::EdgeCoffeeMachine::instance().setIsRecognizing(false);
    }
    if (m_is_waiting_for_face && !result.det_res.empty()) {
        // Stop waiting to prevent double-triggering
        m_is_waiting_for_face = false;

        Qul::PlatformInterface::log("[FaceRecognitionApp] Face detected! Triggering enrollment now.\n");

        // Trigger the one-shot enrollment event in the core task
        auto recognition_task = m_recognition->get_recognition_task();
        xEventGroupSetBits(recognition_task->get_event_group(), 
                           who::recognition::WhoRecognitionCore::ENROLL);
    }
}

void FaceRecognitionApp::recognition_result_cb(const std::string &result)
{
    Qul::PlatformInterface::log("[FaceRecognitionApp] Result: %s\n", result.c_str());
    // 1. Handle Enrollment Failure (Retry Logic)
    if (result.find("Failed to enroll") != std::string::npos) {
        Qul::PlatformInterface::log("[FaceRecognitionApp] Enrollment failed (bad image?). Retrying...\n");
        // Go back to waiting for a valid face frame
        m_is_waiting_for_face = true;
        return;
    }

    // 2. Handle Existing User Identification / Enrollment Success
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
                // Enrollment successful, we stop waiting (flag is already false)
                Qul::PlatformInterface::log("[FaceRecognitionApp] Enrolling user with ID: %d\n", id);
                Logic::EdgeCoffeeMachine::instance().createUser(id);
            } else {
                Qul::PlatformInterface::log("[FaceRecognitionApp] Identifying user with ID: %d\n", id);
                Logic::EdgeCoffeeMachine::instance().identifyUser(id);
            }
        }
    } else if (result.find("Unknown face") != std::string::npos) {
        Qul::PlatformInterface::log("[FaceRecognitionApp] Unknown face detected, logging out.\n");
        Logic::EdgeCoffeeMachine::instance().logoutUser();
    } else if (result.find("enrolled") != std::string::npos) { 
        // Catch generic "id: X enrolled" message if format differs
        Qul::PlatformInterface::log("[FaceRecognitionApp] User enrolled successfully.\n");
    }

    Logic::EdgeCoffeeMachine::instance().setIsRecognizing(false);
}

FaceRecognitionApp* start_face_recognition()
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
    
    return app;
}

void FaceRecognitionApp::trigger_enrollment()
{
    m_is_waiting_for_face = true;
    Qul::PlatformInterface::log("[FaceRecognitionApp] Waiting for face to enroll...\n");
}