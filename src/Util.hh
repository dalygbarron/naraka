#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <spdlog/spdlog.h>

#define gl(call) gl##call;checkGl(__LINE__, __FILE__)

void checkGl(int line, char const *file);

/**
 * Just contains random utility crap.
 */
namespace Util {
    using namespace glm;

    /**
     * Reads in a file and puts the whole thing in a string.
     * @param file is the name of the file to load from.
     * @return the created string, or nothing if it's screwed.
     */
    std::optional<std::string> loadString(char const *file);

    /**
     * A nice rectangle consisting of a position vector and a size vector.
     */
    class Rect {
        public:
            glm::vec2 pos;
            glm::vec2 size;

            /**
             * Default constructor zeroes everything.
             */
            Rect();

            /**
             * Sets every number.
             * @param x is the distance from left.
             * @param y is the distance from bottom.
             * @param w is the width.
             * @param h is the height.
             */
            Rect(float x, float y, float w, float h);

            /**
             * Gives you a flipped version of this rect without modifying this.
             * This means that if it's a normal rect, the width and height are
             * now gonna be negative which possibly yields weird results in a
             * lot of cases LOL.
             * @param vertical whether to flip the rect on vertical axis.
             * @param horizontal whether to flip the rect on horizontal axis.
             * @return a new rect that is flipped as you desire.
             */
            Rect flipped(bool vertical, bool horizontal);
    };

    typedef glm::uvec4 Colour;

    Colour const WHITE = Colour(255, 255, 255, 255);
    Colour const BLACK = Colour(0, 0, 0, 255);
    Colour const GREEN = Colour(0, 255, 0, 255);
};