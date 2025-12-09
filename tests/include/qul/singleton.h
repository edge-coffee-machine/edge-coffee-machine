#pragma once

namespace Qul {
template <typename T>
class Singleton {
protected:
    Singleton() = default;
public:
    static T &instance() {
        static T inst;
        return inst;
    }
};
}
