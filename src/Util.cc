#include "Util.hh"
#include <GL/glew.h>
#include <spdlog/spdlog.h>

void checkGl(int line, char const *file) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        spdlog::error("gl error 0x{:x} at {}:{}", err, file, line);
    }
}

Util::Rect::Rect() {
    pos.x = 0;
    pos.y = 0;
    size.x = 0;
    size.y = 0;
}

Util::Rect::Rect(float x, float y, float w, float h) {
    pos.x = x;
    pos.y = y;
    size.x = w;
    size.y = h;
}

Util::Rect Util::Rect::flipped(bool vertical, bool horizontal) {
    return Rect(
        pos.x + (vertical ? size.x : 0),
        pos.y + (horizontal ? size.y : 0),
        vertical ? size.x : -size.x,
        horizontal ? size.y : -size.y
    );
}