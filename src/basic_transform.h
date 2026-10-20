#ifndef BASIC_TRANSFORM_H
#define BASIC_TRANSFORM_H

#include <cassert>  // vector_util.h uses assert
#include "vector_util.h"
#include <SFML/System/Vector2.hpp>

// A transform supporting only scale and translation.
// Convention (same as the usual affine S -> R -> T order, minus rotation):
//   scale is applied FIRST, then translation:  p' = scale * p + translation
// Scaling first means the scale happens about the local origin, and the translation
// is not itself scaled (otherwise moving an object by +10 with scale 2 would move it by 20).
struct BasicTransform {
    constexpr BasicTransform(const sf::Vector2f& scale = {1.f, 1.f},
                             const sf::Vector2f& translation = {0.f, 0.f})
        : scale(scale), translation(translation) {}

    // Point: homogeneous w = 1, so translation applies.
    constexpr sf::Vector2f applyPoint(const sf::Vector2f& p) const {
        return scale * p + translation;
    }

    // Vector: homogeneous w = 0, so translation does NOT apply (only scale).
    constexpr sf::Vector2f applyVector(const sf::Vector2f& p) const { return scale * p; }

    // Exact (bitwise-style) float comparison, as the problem asks.
    constexpr bool operator==(const BasicTransform& other) const {
        return scale == other.scale && translation == other.translation;
    }

    sf::Vector2f scale;
    sf::Vector2f translation;
};

#endif
