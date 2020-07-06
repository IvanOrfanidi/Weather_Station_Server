#pragma once

template <class T>
class NonMovable {
public:
    NonMovable(const NonMovable&&) = delete;
    T& operator=(T&&) = delete;

protected:
    NonMovable() = default;
    ~NonMovable() = default;
};
