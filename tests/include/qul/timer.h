#pragma once

#include <functional>

namespace Qul {
class Timer {
public:
    Timer() = default;
    void start(int) {}
    void stop() {}
    void setCallback(std::function<void()> cb) { m_cb = std::move(cb); }
private:
    std::function<void()> m_cb;
};
}
