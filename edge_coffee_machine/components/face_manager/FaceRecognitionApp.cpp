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
    : who::app::WhoRecognitionAppTerm(frame_cap), 
      m_is_paused(true) // Set to true: Paused by default
{
    auto recognition_task = m_recognition->get_recognition_task();
    auto detect_task = m_recognition->get_detect_task();

    recognition_task->set_detect_result_cb(
        std::bind(&FaceRecognitionApp::detect_result_cb, this, std::placeholders::_1));

    detect_task->set_detect_result_cb(
        std::bind(&FaceRecognitionApp::detect_result_cb, this, std::placeholders::_1));

    Qul::PlatformInterface::log("[FaceRecognitionApp] Initialized for ESP32-P4 (Paused by default).\n");
}

void FaceRecognitionApp::detect_result_cb(const who::detect::WhoDetect::result_t &result)
{
    if (m_is_paused || m_is_enrolling || m_is_recognizing) {
        return;
    }

   if (!result.det_res.empty()) {
        auto recognition_task = m_recognition->get_recognition_task();

        if (m_current_mode == MODE_ENROLL) {
            Qul::PlatformInterface::log("[FaceRecognitionApp] Face detected. Starting Enrollment...\n");
            m_is_enrolling = true; // Set busy flag
            xEventGroupSetBits(recognition_task->get_event_group(), 
                               who::recognition::WhoRecognitionCore::ENROLL);
        }
        else if (m_current_mode == MODE_RECOGNIZE) {
            Qul::PlatformInterface::log("[FaceRecognitionApp] Face detected. Starting Recognition...\n");
            m_is_recognizing = true; // Set busy flag
            xEventGroupSetBits(recognition_task->get_event_group(), 
                               who::recognition::WhoRecognitionCore::RECOGNIZE);
        }
    }
}

void FaceRecognitionApp::recognition_result_cb(const std::string &result)
{
    Qul::PlatformInterface::log("[FaceRecognitionApp] Engine Result: %s\n", result.c_str());

    if (m_is_recognizing) {
        m_is_recognizing = false; // Clear busy flag

        // Case 1: Success "ID: 123"
        size_t id_pos = result.find("id: ");
        if (id_pos != std::string::npos) {
            // Parse the ID number from the string
            int id = std::stoi(result.substr(id_pos + 4));
            
            Qul::PlatformInterface::log("[FaceRecognitionApp] Success! Identified User ID: %d\n", id);
            Logic::EdgeCoffeeMachine::instance().identifyUser(id);
            Logic::EdgeCoffeeMachine::instance().setIsRecognizing(false);

            // Stop looking once found
            FaceRecognitionApp::pause(); 
            m_current_mode = MODE_IDLE;
        } 
        // Case 2: Unknown Face
        else if (result.find("who?") != std::string::npos) {
            Qul::PlatformInterface::log("[FaceRecognitionApp] Unknown face detected.\n");
            Logic::EdgeCoffeeMachine::instance().logoutUser();
            
            // Optional: Pause after unknown? Or keep trying?
            // FaceRecognitionApp::pause();
        }
        // Case 3: Failed (Blurry/Bad Angle)
        else {
            // Implicitly retry: We cleared the 'm_is_recognizing' flag 
            // and didn't pause, so 'detect_result_cb' will trigger again on the next frame.
        }
    }

    // ---------------------------------------------------------
    // HANDLE ENROLLMENT
    // ---------------------------------------------------------
    else if (m_is_enrolling) {
        size_t id_pos = result.find("id: ");
        if (id_pos != std::string::npos) {
            // Parse the ID number from the string
            int id = std::stoi(result.substr(id_pos + 4));
            Qul::PlatformInterface::log("[FaceRecognitionApp] Enrollment Confirmed!\n");
            
            // Create user
            Logic::EdgeCoffeeMachine::instance().createUser(id);
            Logic::EdgeCoffeeMachine::instance().setIsRecognizing(false);
            
            // Clean up
            m_is_enrolling = false;
            m_current_mode = MODE_IDLE;
            FaceRecognitionApp::pause(); 
        } 
        else if (result.find("Failed") != std::string::npos) {
            Qul::PlatformInterface::log("[FaceRecognitionApp] Enrollment failed (blurry). Retrying...\n");
            m_is_enrolling = false; // Retry immediately
        }
    }
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
    m_current_mode = MODE_ENROLL; // Set intent to Enroll
    FaceRecognitionApp::resume();
    Qul::PlatformInterface::log("[FaceRecognitionApp] Mode set to ENROLL. Waiting for face...\n");
}

void FaceRecognitionApp::trigger_recognition()
{
    Logic::EdgeCoffeeMachine::instance().setIsRecognizing(true);
    m_current_mode = MODE_RECOGNIZE; // Set intent to Recognize
    FaceRecognitionApp::resume();
    Qul::PlatformInterface::log("[FaceRecognitionApp] Mode set to RECOGNIZE. Waiting for face...\n");
}

// Fixed: Return bool to match base class signature
bool FaceRecognitionApp::pause()
{
    m_is_paused = true;
    Qul::PlatformInterface::log("[FaceRecognitionApp] Detection paused.\n");
    return true;
}

// Fixed: Return bool to match base class signature
bool FaceRecognitionApp::resume()
{
    m_is_paused = false;
    Qul::PlatformInterface::log("[FaceRecognitionApp] Detection resumed.\n");
    return true;
}