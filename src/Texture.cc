#include "Texture.hh"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>

Texture::Texture(GLuint id, glm::uvec2 size):
    id(id),
    size(size),
    invSize(1.0f / size.x, 1.0f / size.y)
{
    // nothing else right now lol.
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

std::optional<Texture> Texture::create(char const *filename) {
    GLuint id;
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface) {
        spdlog::error("Couldn't load image. SDL_Image: {}", IMG_GetError());
        return {};
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    int mode = GL_RGB;
    if (surface->format->BytesPerPixel == 4) mode = GL_RGBA;
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        mode,
        surface->w,
        surface->h,
        0,
        mode,
        GL_UNSIGNED_BYTE,
        surface->pixels
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return Texture(id, glm::uvec2(surface->w, surface->h));
}