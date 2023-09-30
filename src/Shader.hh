#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "Texture.hh"

/**
 * Stores info about a gl shader and lets you use it.
 */
class Shader {
    public:
        /**
         * Just stores info that we keep regarding a sampler.
         */
        class Sampler {
            public:
                GLint sampler;
                GLint invSize;
        };

        /**
         * Interface for something that a shader can draw to the screen.
         */
        class Drawable {
            public:
                GLuint const vertexBuffer;
                GLuint const uvBuffer;
                GLuint const colourBuffer;
                unsigned int const nTextures;
                Texture const * const * const textures;

                /**
                 * Creates the drawable by just passing in all it needs.
                 * @param vertexBuffer handle to the vertex buffer which
                 *        be valid and non zero.
                 * @param uvBuffer handle to the uv buffer. If this is 0 then
                 *        it will be assumed to be unneeded.
                 * @param colourBuffer handle to the colour buffer. Can be 0 for
                 *        unused.
                 * @param nTextures the number of textures the drawable has
                 *        associated with it.
                 * @param textures pointer to start of array of pointers to
                 *        textures. This array now belongs to this object so
                 *        don't try to free it or whatever. The textures will
                 *        not be freed by this thought obviously.
                */
                Drawable(
                    GLuint vertexBuffer,
                    GLuint uvBuffer,
                    GLuint colourBuffer,
                    unsigned int nTextures,
                    Texture const * const *textures
                );

                /**
                 * Frees the buffers, but not the textures.
                 */
                virtual ~Drawable();

                /**
                 * This is called before the shader draws the thing, allowing
                 * it to update it's shit if it wants.
                 */
                virtual unsigned int predraw() = 0;
        };

    private:
        /**
         * Just inserts all the content of a shader.
         * @param program full shader program, managed by this.
         * @param posAttr vertex pos shader attribute.
         * @param uvAttr vertex uv shader attribute.
         * @param colAttr vertex colour shader attribute.
         * @param invCanvasUni inverse canvas size uniform location
         * @param timeUni time uniform location.
         * @param nSamplers number of samplers shader is meant to have.
         * @param samplers pointer to array of sampler objects. managed by this.
         * @param nExtras number of extra uniforms.
         * @param extras pointer to array of extra uniforms, managed by this.
         */
        Shader(
            GLuint program,
            GLint posAttr,
            GLint uvAttr,
            GLint colAttr,
            GLint invCanvasUni,
            GLint timeUni,
            unsigned int nSamplers,
            Sampler *samplers,
            unsigned int nExtras,
            GLint *extras
        );

    public:
        GLuint const program;
        GLint const posAttr;
        GLint const uvAttr;
        GLint const colAttr;
        GLint const invCanvasUni;
        GLint const timeUni;
        unsigned int const nSamplers;
        Sampler const * const samplers;
        unsigned int nExtras;
        GLint const * const extras;
        constexpr static char const * const defaultSampler = "tex";
        constexpr static char const * const defaultFrag = R""""(
        #version 140
        uniform sampler2D tex;
        in highp vec2 vTextureCoord;
        in mediump vec4 vColour;
        out vec4 fragColor;
        void main() {
            fragColor = texture(tex, vTextureCoord) * vColour;
        }
        )"""";
        constexpr static char const * const defaultVert = R""""(
        #version 140
        in vec2 position;
        in vec2 uv;
        in vec4 colour;
        uniform vec2 canvasInv;
        uniform vec2 inv_tex;
        out highp vec2 vTextureCoord;
        out highp vec2 vPosition;
        out mediump vec4 vColour;
        void main() {
            gl_Position = vec4(
            position * canvasInv * 2.0 - vec2(1.0, 1.0), 0.0, 1.0);
            vTextureCoord = uv * inv_tex;
            vPosition = position;
            vColour = colour;
        }
        )"""";

        /**
         * Destroys the shader program and managed memory.
         */
        ~Shader();

        /**
         * Draws a drawable on the screen.
         * @param drawable is the fucker that is about to get drawn.
         */
        void draw(Drawable &drawable);

        /**
         * Creates a shader. All of these values have defaults if you want to
         * create a default shader.
         * @param fragSrc is the source code of the frag shader.
         * @param vertSrc is the source code of the vert shader.
         * @param nSamplers is the number of sampler names passed. Defaults to 1
         *        under the assumption samplers will also be defaulted. You can
         *        pass 0 if you don't want any btw.
         * @param samplers is a list of sampler names to set up uniforms for.
         *        Defaults to one default sampler called 'texture'. Make sure
         *        the number of these matches with nSamplers above.
         * @param nExtras is the number of extra uniforms. Defaults to 0.
         * @param extras is the list of extra uniform names to set up. Match the
         *        number with nExtras obviously. Defaults to NULL.
         * @return an optional shader. If the creation failed then there will
         *         be no shader and an error will have been logged already.
         */
        static std::optional<Shader> create(
            char const *fragSrc = defaultFrag,
            char const *vertSrc = defaultVert,
            unsigned int nSamplers = 1,
            char const * const *samplers = &defaultSampler,
            unsigned int nExtras = 0,
            char const * const *extras = NULL
        );
};