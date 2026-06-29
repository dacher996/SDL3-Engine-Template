#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

namespace Engine {
    static constexpr float PI = 3.1415927410125732421875f;
    
    /// Convert degrees to radians
    constexpr float deg2rad(float degrees) { return degrees * (PI / 180.0f); }

    /// Convert radians to degrees
    constexpr float rad2deg(float radians) { return radians * (180.0f / PI); }
} // namespace Engine

#endif // ENGINE_UTILS_H
