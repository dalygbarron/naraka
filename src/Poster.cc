#include "Poster.hh"

/**
 * Fills an array of bytes with six vertices worth of colour data.
 * @param colour is the colour to fill with.
 * @param array is an array of bytes, needs to be at least 24 entries long.
 */
void fillColourArray(Util::Colour colour, GLubyte *array) {
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
    gl(BindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    gl(BufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW));
    if (uvBuffer) {
        gl(BindBuffer(GL_ARRAY_BUFFER, uvBuffer));
        gl(BufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLshort), uvs, GL_STATIC_DRAW));
    }
    if (colourBuffer) {
        gl(BindBuffer(GL_ARRAY_BUFFER, colourBuffer));
        gl(BufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLbyte), colours, GL_STATIC_DRAW));
    }
    spdlog::info("Poster constructor");
}

Poster::~Poster() {
    spdlog::info("Poster deconstructor");
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
    if (!makeBuffers(buffers)) return {};
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