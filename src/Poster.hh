#pragma once

#include <spdlog/spdlog.h>
#include "Util.hh"
#include "Shader.hh"

class Poster: public Shader::Drawable {
    private:
        GLfloat vertices[12];
        GLshort uvs[12];
        GLbyte colours[24];

        /**
         * Creates a poster, puts in most of the stuff that it uses, but this
         * then creates it's arrays of data and puts them into the gl buffers.
         * @param vertexBuffer the poster's vertex buffer.
         * @param uvBuffer the poster's uv buffer, can be 0 to ignore.
         * @param colourBuffer the poster's colour buffer, can be 0 to ignore.
         * @param nTextures number of textures the poster has.
         * @param textures list of pointers to the textures.
         * @param vertexRect defines the dimensions of the poster on screen.
         * @param uvRect defines the uv mapping of the rectangle.
         * @param colour vertex colour of the vertices.
         */
        Poster(
            GLuint vertexBuffer,
            GLuint uvBuffer,
            GLuint colourBuffer,
            unsigned int nTextures,
            Texture const * const *textures,
            Util::Rect vertexRect,
            Util::Rect uvRect,
            Util::Colour colour
        );

    public:
        /**
         * Destructor.
         */
        virtual ~Poster();

        virtual unsigned int predraw() override;

        /**
         * Fills an array of some type with vertex data based on the given rect.
         * It's templated because I do vertex positions as floats and uv data
         * as shorts.
         * @param rect defines the locations of the vertices.
         * @param array is where the data is saved. It must fit at least 12 entries.
         */
        template <typename T> static void fillVertexArray(
            Util::Rect rect,
            T *array
        ) {
            array[0] = rect.pos.x;
            array[1] = rect.pos.y;
            array[2] = rect.pos.x + rect.size.x;
            array[3] = rect.pos.y;
            array[4] = rect.pos.x;
            array[5] = rect.pos.y + rect.size.y;
            array[6] = rect.pos.x + rect.size.x;
            array[7] = rect.pos.y;
            array[8] = rect.pos.x + rect.size.x;
            array[9] = rect.pos.y + rect.size.y;
            array[10] = rect.pos.x;
            array[11] = rect.pos.y + rect.size.y;
        }

        /**
         * Creates a poster.
         * @param rect is the dimensions of the poster.
         * @param uv defines the uv positions of the poster.
         * @param colour is the vertex colour of the poster.
         * @param nTextures is the number of textures the poster has.
         * @param textures is a reference to an array of pointers to textures
         *        that the poster will be able to use. This array will be copied
         *        so you can define it on the stack or whatever, make sure to
         *        delete your version if you need to. In the end the poster will
         *        not delete the textures, either.
         */
        static std::optional<Poster> create(
            Util::Rect rect,
            Util::Rect uv = Util::Rect(),
            Util::Colour colour = Util::WHITE,
            unsigned int nTextures = 0,
            Texture const * const *textures = NULL
        );
};