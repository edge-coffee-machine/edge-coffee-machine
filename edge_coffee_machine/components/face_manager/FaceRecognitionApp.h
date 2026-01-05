#pragma once

#include "who_recognition_app_term.hpp"

class FaceRecognitionApp : public who::app::WhoRecognitionAppTerm {
public:
    FaceRecognitionApp(who::frame_cap::WhoFrameCap *frame_cap);
    virtual ~FaceRecognitionApp() = default;

protected:
    void recognition_result_cb(const std::string &result) override;
};

void start_face_recognition();
