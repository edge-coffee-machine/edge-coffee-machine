#pragma once

namespace Qul {

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    static T* s_instance;

    static T &instance() {
        if (!s_instance) {
            s_instance = new T();
        }
        return *s_instance;
    }

    // For testing: resets the instance
    static void resetInstance() {
        if (s_instance) {
            delete s_instance;
            s_instance = nullptr;
        }
    }
};

// Inicialización del puntero estático (necesario para templates)
template<typename T>
T* Singleton<T>::s_instance = nullptr;

}