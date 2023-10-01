#pragma once

#include "Shader.hh"
#include "Util.hh"

/**
 * Batches rendering of sproits.
 */
class Batch: public Shader::Drawable {
    private:
        unsigned int const max;
        GLfloat * const vertices;
        GLshort * const uvs;
        GLubyte * const colours;
        unsigned int n = 0;

        /**
         * Puts in all the stuff.
         * @param vertexBuffer vertex buffer.
         * @param uvBuffer uv buffer.
         * @param colourBuffer colour buffer.
         * @param textures is the textures list but it should just be one.
         * @param max is max items.
         * @param vertices is empty vertex data array of size matching max.
         * @param uvs is empty uv data array of size matching max.
         * @param colours is empty colour data array of size matching max.
         */
        Batch(
            GLuint vertexBuffer,
            GLuint uvBuffer,
            GLuint colourBuffer,
            Texture const * const *textures,
            unsigned int max,
            GLfloat *vertices,
            GLshort *uvs,
            GLubyte *colours
        );

    public:
        /**
         * deletes shit.
         */
        virtual ~Batch();

        unsigned int predraw() override;

        /**
         * Clears the batch to draw new stuff.
         */
        void clear();

        /**
         * Adds a sprite to the batch by it's constituent parts.
         * @param src is the source texture bits to use.
         * @param l is the distance from left of screen to start draw.
         * @param b is the distance from bottom of screen to start draw.
         * @param r is the distance from left of screen to stop draw.
         * @param t is the distance from bottom of screen to stop draw.
         */
        void addComp(
            Util::Rect src,
            float l,
            float b,
            float r,
            float t,
            Util::Colour colour = Util::WHITE
        );

        /**
         * Creates a batch that uses a given texture.
         * @param texture is the texture it draws with.
         * @param max is the maximum number of items it can draw.
         * @param hint is a rendering hint for opengl. Defaults to dynamic draw.
         */
        static std::optional<Batch> create(
            Texture const *texture,
            unsigned int max,
            GLenum hint = GL_DYNAMIC_DRAW
        );
};