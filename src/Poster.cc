#include "Poster.hh"

/**
 * Fills an array of bytes with six vertices worth of colour data.
 * @param colour is the colour to fill with.
 * @param array is an array of bytes, needs to be at least 24 entries long.
 */
void fillColourArray(Util::Colour colour, GLbyte *array) {
    for (int i = 0; i < 6; i++) {
        array[i * 4] = colour.r;
        array[i * 4 + 1] = colour.g;
        array[i * 4 + 2] = colour.b;
        array[i * 4 + 3] = colour.a;
    }
}

Poster::Poster(
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint colourBuffer,
    unsigned int nTextures,
    Texture const * const *textures,
    Util::Rect vertexRect,
    Util::Rect uvRect,
    Util::Colour colour
): Drawable(vertexBuffer, uvBuffer, colourBuffer, nTextures, textures) {
    fillVertexArray<GLfloat>(vertexRect, vertices);
    fillVertexArray<GLshort>(uvRect.flipped(true, false), uvs);
    fillColourArray(colour, colours);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 12, vertices, GL_STATIC_DRAW);
    if (uvBuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, 12, uvs, GL_STATIC_DRAW);
    }
    if (colourBuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
        glBufferData(GL_ARRAY_BUFFER, 24, colours, GL_STATIC_DRAW);
    }
}

Poster::~Poster() {
}

unsigned int Poster::predraw() {
    return 6;
}

std::optional<Poster> Poster::create(
    Util::Rect rect,
    Util::Rect uv,
    Util::Colour colour,
    unsigned int nTextures,
    Texture const * const *textures
) {
    GLuint buffers[3];
    glGenBuffers(3, buffers);
    for (int i = 0; i < 3; i++) {
        if (!buffers[i]) {
            spdlog::error("Failed to set up buffers for poster");
            return {};
        }
    }
    return Poster(
        buffers[0],
        buffers[1],
        buffers[2],
        nTextures,
        textures,
        rect,
        uv,
        colour
    );
}