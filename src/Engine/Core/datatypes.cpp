#include "Engine/Core/datatypes.h"

namespace Engine {
    const Vec2i Vec2i::ZERO = Vec2i(0);
    const Vec2i Vec2i::ONE = Vec2i(1);

    const Vec2f Vec2f::ZERO = Vec2f(0.0f);
    const Vec2f Vec2f::ONE = Vec2f(1.0f);

    const Vec3i Vec3i::ZERO = Vec3i(0);
    const Vec3i Vec3i::ONE = Vec3i(1);

    const Vec3f Vec3f::ZERO = Vec3f(0.0f);
    const Vec3f Vec3f::ONE = Vec3f(1.0f);

    const Recti Recti::ZERO = Recti(0, 0, 0, 0);
    const Rectf Rectf::ZERO = Rectf(0, 0, 0, 0);

    Vec2i::operator SDL_Point() const { return SDL_Point(x, y); }
    Vec2f::operator SDL_FPoint() const { return SDL_FPoint(x, y); }
    Recti::operator SDL_Rect() const { return SDL_Rect({x, y, w, h}); }
    Rectf::operator SDL_FRect() const { return SDL_FRect({x, y, w, h}); }

    Vec2i Vec2i::operator*(const int &val) const { return {x * val, y * val}; }
    Vec2f Vec2f::operator*(const float &val) const { return {x * val, y * val}; }
    Vec3i Vec3i::operator*(const int &val) const { return {x * val, y * val, z * val}; }
    Vec3f Vec3f::operator*(const float &val) const { return {x * val, y * val, z * val}; }

    bool Recti::IsIntersecting(const Recti &other) const {
        SDL_Rect result;
        SDL_Rect thisRect = *this;
        SDL_Rect otherRect = other;

        return SDL_GetRectIntersection(&thisRect, &otherRect, &result);
    }

    bool Rectf::IsIntersecting(const Rectf &other) const {
        SDL_FRect result;
        SDL_FRect thisRect = *this;
        SDL_FRect otherRect = other;

        return SDL_GetRectIntersectionFloat(&thisRect, &otherRect, &result);
    }
}
