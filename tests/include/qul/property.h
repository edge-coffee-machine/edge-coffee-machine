#pragma once

namespace Qul {
template <typename T>
class Property {
public:
    Property() = default;
    Property(const T &v) : m_value(v) {}
    void setValue(const T &v) { m_value = v; }
    T value() const { return m_value; }
    T &valueRef() { return m_value; }
private:
    T m_value{};
};
}
