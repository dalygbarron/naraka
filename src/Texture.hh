#pragma once

#include <optional>
#include <glm/vec2.hpp>
#include <SDL_opengl.h>

/**
 * Basic store for gpu texture and it's size.
 */
class Texture {
    private:
        /**
         * Private constructor to be called by creation function.
         * @param id is the texture's opengl id.
         * @param size is the texture's dimensions as pixels.
         */
        Texture(GLuint id, glm::uvec2 size);

    public:
        GLuint const id;
        glm::uvec2 const size;
        glm::vec2 const invSize;

        /**
         * Frees the opengl stuff.
         */
        ~Texture();

        /**
         * Binds the texture to the opengl context or whatever.
         */
        void bind() const;

        /**
         * Creates a texture from an image file.
         * @param filename is where to load the image.
         * @return */
        static std::optional<Texture> create(char const *filename);
};