#include "Batch.hh"
#include <stdio.h>

Batch::Batch(
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint colourBuffer,
    Texture const * const *textures,
    unsigned int max,
    GLfloat *vertices,
    GLshort *uvs,
    GLubyte *colours
):
    Drawable(vertexBuffer, uvBuffer, colourBuffer, 1, textures),
    max(max),
    vertices(vertices),
    uvs(uvs),
    colours(colours)
{
    // gamer.
}

Batch::~Batch() {
    // TODO: delete the textures array array because we create it dynamically.
}

unsigned int Batch::predraw() {
    gl(BindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    gl(BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 12 * n, vertices));
    gl(BindBuffer(GL_ARRAY_BUFFER, uvBuffer));
    gl(BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLshort) * 12 * n, uvs));
    gl(BindBuffer(GL_ARRAY_BUFFER, colourBuffer));
    gl(BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLubyte) * 24 * n, colours));
    return n * 6;
}

void Batch::clear() {
    n = 0;
}

void Batch::addComp(
    Util::Rect src,
    float l,
    float b,
    float r,
    float t,
    Util::Colour colour
) {
    if (n >= max) {
        spdlog::warn("Batch is full");
        return;
    }
    unsigned int offset = n * 12;
    vertices[offset] = l;
    vertices[offset + 1] = b;
    vertices[offset + 2] = r;
    vertices[offset + 3] = b;
    vertices[offset + 4] = l;
    vertices[offset + 5] = t;
    vertices[offset + 6] = r;
    vertices[offset + 7] = b;
    vertices[offset + 8] = r;
    vertices[offset + 9] = t;
    vertices[offset + 10] = l;
    vertices[offset + 11] = t;
    uvs[offset] = src.pos.x;
    uvs[offset + 1] = src.pos.y + src.size.y;
    uvs[offset + 2] = src.pos.x + src.size.x;
    uvs[offset + 3] = src.pos.y + src.size.y;
    uvs[offset + 4] = src.pos.x;
    uvs[offset + 5] = src.pos.y;
    uvs[offset + 6] = src.pos.x + src.size.x;
    uvs[offset + 7] = src.pos.y + src.size.y;
    uvs[offset + 8] = src.pos.x + src.size.x;
    uvs[offset + 9] = src.pos.y;
    uvs[offset + 10] = src.pos.x;
    uvs[offset + 11] = src.pos.y;
    unsigned int colourOffset = n * 24;
    for (int i = 0; i < 6; i++) {
        colours[colourOffset + i * 4] = colour.r;
        colours[colourOffset + i * 4 + 1] = colour.g;
        colours[colourOffset + i * 4 + 2] = colour.b;
        colours[colourOffset + i * 4 + 3] = colour.a;
    }
    n++;
}

std::optional<Batch> Batch::create(
    Texture const *texture,
    unsigned int max,
    GLenum hint
) {
    GLuint buffers[3];
    if (!makeBuffers(buffers)) return {};
    GLfloat *vertices = new GLfloat[max * 12];
    GLshort *uvs = new GLshort[max * 12];
    GLubyte *colours = new GLubyte[max * 24];
    gl(BindBuffer(GL_ARRAY_BUFFER, buffers[0]));
    gl(BufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * 12 * max,
        vertices,
        GL_DYNAMIC_DRAW
    ));
    gl(BindBuffer(GL_ARRAY_BUFFER, buffers[1]));
    gl(BufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLshort) * 12 * max,
        uvs,
        GL_DYNAMIC_DRAW
    ));
    gl(BindBuffer(GL_ARRAY_BUFFER, buffers[2]));
    gl(BufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLubyte) * 24 * max,
        colours,
        GL_DYNAMIC_DRAW
    ));
    Texture const * const *textures = new Texture*[1];
    textures = &texture;
    return Batch(
        buffers[0],
        buffers[1],
        buffers[2],
        textures,
        max,
        vertices,
        uvs,
        colours
    );
}