#pragma once

#include "who_recognition_app_term.hpp"

class FaceRecognitionApp : public who::app::WhoRecognitionAppTerm {
public:
    FaceRecognitionApp(who::frame_cap::WhoFrameCap *frame_cap);
    virtual ~FaceRecognitionApp() = default;
    void trigger_enrollment();
    void trigger_recognition();
    
    // Kept as bool to satisfy base class override requirements
    bool pause() override;
    bool resume() override;
    enum AppMode { MODE_IDLE, MODE_ENROLL, MODE_RECOGNIZE };
    AppMode m_current_mode = MODE_IDLE;
protected:
    void recognition_result_cb(const std::string &result) override;
private:
    bool m_is_paused; // Restored custom flag
    bool m_is_enrolling = false;
    bool m_is_recognizing = false;

    // Callback that receives detection results (bounding boxes)
    void detect_result_cb(const who::detect::WhoDetect::result_t &result);
};

FaceRecognitionApp* start_face_recognition();