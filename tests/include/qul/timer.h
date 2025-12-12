#pragma once

#include <functional>

namespace Qul {

    // A fake Timer class that allows manual triggering for tests
    class Timer {
    public:
        using Callback = std::function<void()>;

        Timer() : m_interval(0), m_singleShot(false), m_running(false) {}

        void setSingleShot(bool single) {
            m_singleShot = single;
        }

        void setInterval(int ms) {
            m_interval = ms;
        }

        void onTimeout(Callback cb) {
            m_callback = cb;
        }

        void start() {
            m_running = true;
        }

        void stop() {
            m_running = false;
        }

        // --- TEST HELPER ---
        // Call this from the test to simulate time passing instantly
        void forceTimeout() {
            if (m_running && m_callback) {
                m_callback();
                if (m_singleShot) {
                    m_running = false;
                }
            }
        }

        bool isRunning() const { return m_running; }
        int getInterval() const { return m_interval; }

    private:
        int m_interval;
        bool m_singleShot;
        bool m_running;
        Callback m_callback;
    };
}