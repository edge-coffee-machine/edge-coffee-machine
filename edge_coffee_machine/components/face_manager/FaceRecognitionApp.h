#pragma once

#include "who_recognition_app_term.hpp"

class FaceRecognitionApp : public who::app::WhoRecognitionAppTerm {
public:
    FaceRecognitionApp(who::frame_cap::WhoFrameCap *frame_cap);
    virtual ~FaceRecognitionApp() = default;
    void trigger_enrollment();
    void pause();
    void resume();

protected:
    void recognition_result_cb(const std::string &result) override;
private:
    bool m_is_waiting_for_face;
    bool m_is_paused;
    int m_frame_counter; // <--- Add this debug counter
    // Callback that receives detection results (bounding boxes)
    void detect_result_cb(const who::detect::WhoDetect::result_t &result);
};

FaceRecognitionApp* start_face_recognition();
