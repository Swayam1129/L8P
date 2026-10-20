#ifndef SPRITE_H
#define SPRITE_H

#include "basic_transform.h"
#include "vector_util.h"
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Sprite {  // could derive from sf::Drawable if you're willing to pay for a vptr, but not
                // necessary.
public:
    // Whole texture: subrectangle is [0, 0] -> texture size.
    Sprite(const sf::Texture& texture)
        : Sprite(texture, {0.f, 0.f}, sf::Vector2f(texture.getSize())) {}

    // Subrectangle [texMinPoint, texMaxPoint] of the texture (in texel/pixel coords).
    Sprite(const sf::Texture& texture, const sf::Vector2f& texMinPoint,
           const sf::Vector2f& texMaxPoint)
        : mTexture(&texture),
          mVertexArray(sf::PrimitiveType::Triangles, DEFAULT_VERTICES.size()),
          mMinPointTex(texMinPoint),
          mMaxPointTex(texMaxPoint),
          mOrigin(0.f, 0.f),
          mTransform() {}

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!mTexture) return;
        recomputeVertexArray();  // position/scale/origin may have changed via the accessors
        states.texture = mTexture;
        target.draw(mVertexArray, states);
    }

    sf::Vector2f& position() { return mTransform.translation; }
    const sf::Vector2f& position() const { return mTransform.translation; }
    sf::Vector2f& scale() { return mTransform.scale; }
    const sf::Vector2f& scale() const { return mTransform.scale; }
    sf::Vector2f& origin() { return mOrigin; }
    const sf::Vector2f& origin() const { return mOrigin; }

private:
    // It may help you to write this, which is an array of 6 vertices:
    //   - the first 3 correspond to the vertices of the top-left triangle of a "default rectangle"
    //     ranging from [0, 1] in both dimensions.
    //   - the next 3 correspond to the vertices of the bottom-right triangle. of the default
    //     rectangle.
    // Counterclockwise as seen on screen (SFML: y down), same winding as q2 (a).
    static constexpr std::array<sf::Vector2f, 6> DEFAULT_VERTICES = {
        sf::Vector2f{0.f, 0.f}, sf::Vector2f{0.f, 1.f}, sf::Vector2f{1.f, 0.f},  // top-left
        sf::Vector2f{1.f, 1.f}, sf::Vector2f{1.f, 0.f}, sf::Vector2f{0.f, 1.f},  // bottom-right
    };

    // if mTexture is valid, recomputes the positions and coords of sf::Vertex for each
    // of 6 possible entries in mVertexArray.
    void recomputeVertexArray() const {
        if (!mTexture) return;
        const sf::Vector2f size = mMaxPointTex - mMinPointTex;  // subrectangle size in texels
        mVertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);
        mVertexArray.resize(DEFAULT_VERTICES.size());
        for (std::size_t i = 0; i < DEFAULT_VERTICES.size(); ++i) {
            const sf::Vector2f& d = DEFAULT_VERTICES[i];
            const sf::Vector2f local = d * size;  // corner relative to mMinPointTex
            sf::Vertex& v = mVertexArray[i];
            // shift so the origin sits at (0,0), then scale about it and translate to position
            v.position = mTransform.applyPoint(local - mOrigin);
            // SFML texCoords are in pixels by default, so offset into the subrectangle
            v.texCoords = mMinPointTex + local;
            v.color = sf::Color::White;  // no tinting
        }
    }

    const sf::Texture* mTexture;
    mutable sf::VertexArray mVertexArray;  // geometry, uv coords -- mutable means it can be changed
                                           //  even in const methods.
    sf::Vector2f mMinPointTex;             // AABB for subtexture min point along both x, y
    sf::Vector2f mMaxPointTex;             // AABB for subtexture max point along both x, y
    sf::Vector2f mOrigin;  // position in tex coords of the sprite origin, relative to minPoint
    BasicTransform mTransform;
};

#endif
