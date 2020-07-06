#pragma once

#include "non_copyable.h"
#include "non_movable.h"

template <typename T>
class Singleton
    : public NonCopyable<T>,
      public NonMovable<T> {
public:
    // Get instance class
    static T& getInstance()
    {
        // error class instance
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
};
