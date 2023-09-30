#include "Shader.hh"
#include <math.h>
#include <spdlog/spdlog.h>

int const INV_NAME_BUFFER_SIZE = 128;
int const INV_NAME_OFFSET = 4;
char invNameBuffer[INV_NAME_BUFFER_SIZE] = {'i', 'n', 'v', '_'};

/**
 * Gets a shader's info log as a newly allocated string.
 * @param shader is the shader to get the log for.
 * @returns a newly allocated string with the log which you will need to delete.
 */
char const *getShaderLog(GLuint shader) {
    if (!glIsShader(shader)) {
        spdlog::warn("can't get log of non shader {}", shader);
        return "";
    }
    int maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = new char[maxLength];
    glGetShaderInfoLog(shader, maxLength, NULL, infoLog);
    return infoLog;
}

/**
 * Basically the same as getShaderLog but for programs not individual shaders.
 * @param program is the shader program to get logs for.
 * @returns a newly allocated string containing the stuff. Unless you passed
 *          an invalid program in which case you get an empty string.
 */
char const *getProgramLog(GLuint program) {
    if (!glIsProgram(program)) {
        spdlog::warn("Can't get log of non program {}", program);
        return "";
    }
    int maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = new char[maxLength];
    glGetProgramInfoLog(program, maxLength, NULL, infoLog);
    return infoLog;
}

/**
 * Makes a shader (not a shader program).
 * @param src the source code of the shader.
 * @param type the type of the shader.
 * @return the gl id of the shader. Shader ids cannot be 0, so if 0 is returned
 *         this means that compilation has failed.
 */
GLuint makeShader(char const *src, GLenum type) {
    GLint length = strlen(src);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        char const *log = getShaderLog(shader);
        spdlog::error("Couldn't compile shader {}: {}", shader, log);
        delete[] log;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Shader::Drawable::Drawable(
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint colourBuffer,
    unsigned int nTextures,
    Texture const * const *textures
):
    vertexBuffer(vertexBuffer),
    uvBuffer(uvBuffer),
    colourBuffer(colourBuffer),
    nTextures(nTextures),
    textures(textures)
{
    // that's it.
}

Shader::Drawable::~Drawable() {
    glDeleteBuffers(1, &(vertexBuffer));
    if (uvBuffer) glDeleteBuffers(1, &(uvBuffer));
    if (colourBuffer) glDeleteBuffers(1, &(colourBuffer));
    //delete[] textures;
}

Shader::Shader(
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
):
    program(program),
    posAttr(posAttr),
    uvAttr(uvAttr),
    colAttr(colAttr),
    invCanvasUni(invCanvasUni),
    timeUni(timeUni),
    nSamplers(nSamplers),
    samplers(samplers),
    nExtras(nExtras),
    extras(extras)
{
    // does nothing else rn.
}

Shader::~Shader() {
    glDeleteProgram(program);
    delete[] samplers;
    delete[] extras;
}

void Shader::draw(Drawable &drawable) {
    glUseProgram(program);
    unsigned int n = drawable.predraw();
    glBindBuffer(GL_ARRAY_BUFFER, drawable.vertexBuffer);
    glVertexAttribPointer(posAttr, 2, GL_FLOAT, false, 0, 0);
    if (drawable.uvBuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, drawable.uvBuffer);
        glVertexAttribPointer(uvAttr, 2, GL_SHORT, false, 0, 0);
    }
    if (drawable.colourBuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, drawable.colourBuffer);
        glVertexAttribPointer(colAttr, 4, GL_BYTE, true, 0, 0);
    }
    if (drawable.nTextures > 0) {
        for (int i = 0; i < fmin(drawable.nTextures, nSamplers); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            drawable.textures[i]->bind();
            if (samplers[i].sampler) glUniform1i(samplers[i].sampler, i);
            if (samplers[i].invSize) {
                glUniform2f(
                    samplers[i].invSize,
                    drawable.textures[i]->invSize.x,
                    drawable.textures[i]->invSize.y
                );
            }
        }
    }
    glDrawArrays(GL_TRIANGLES, 0, n);
}

std::optional<Shader> Shader::create(
    char const *fragSrc,
    char const *vertSrc,
    unsigned int nSamplers,
    char const * const *samplers,
    unsigned int nExtras,
    char const * const *extras
) {
    // Set up the shader program.
    GLuint frag = makeShader(fragSrc, GL_FRAGMENT_SHADER);
    if (!frag) return {};
    GLuint vert = makeShader(vertSrc, GL_VERTEX_SHADER);
    if (!vert) {
        glDeleteShader(frag);
        return {};
    }
    GLuint program = glCreateProgram();
    if (!program) {
        spdlog::error("Couldn't create shader program");
        glDeleteShader(frag);
        glDeleteShader(vert);
        return {};
    }
    glAttachShader(program, frag);
    glAttachShader(program, vert);
    glLinkProgram(program);
    GLint linkSuccess = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess != GL_TRUE) {
        char const *programLog = getProgramLog(program);
        spdlog::error("Couldn't link shader program: {}", programLog);
        delete[] programLog;
        glDeleteShader(frag);
        glDeleteShader(vert);
        glDeleteProgram(program);
        return {};
    }
    glDetachShader(program, frag);
    glDetachShader(program, vert);
    glDeleteShader(frag);
    glDeleteShader(vert);
    // Set up engine wide attributes and uniforms.
    glUseProgram(program);
    GLint position = glGetAttribLocation(program, "position");
    GLint uv = glGetAttribLocation(program, "uv");
    GLint colour = glGetAttribLocation(program, "colour");
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(uv);
    glEnableVertexAttribArray(colour);
    GLint invCanvas = glGetUniformLocation(program, "canvasInv");
    GLint time = glGetUniformLocation(program, "time");
    if (invCanvas) {
        // TODO: need to get screen size here.
        glUniform2f(
            invCanvas,
            0.1,
            0.1
        );
    }
    // Set up samplers and extras
    Sampler *samplerUniforms = new Sampler[nSamplers];
    for (int i = 0; i < nSamplers; i++) {
        samplerUniforms[i].sampler = glGetUniformLocation(program, samplers[i]);
        strcpy(invNameBuffer + INV_NAME_OFFSET, samplers[i]);
        samplerUniforms[i].invSize = glGetUniformLocation(
            program,
            invNameBuffer
        );
        if (!samplerUniforms[i].sampler) {
            spdlog::warn("program {} lacks uniform {}", program, samplers[i]);
        }
        if (!samplerUniforms[i].invSize) {
            spdlog::warn("program {} lacks uniform {}", program, invNameBuffer);
        }
    }
    GLint *shaderExtras = new GLint[nExtras];
    for (int i = 0; i < nExtras; i++) {
        shaderExtras[i] = glGetUniformLocation(program, extras[i]);
        if (!shaderExtras[i]) {
            spdlog::warn("program {} lacks uniform {}", program, extras[i]);
        }
    }
    return Shader(
        program,
        position,
        uv,
        colour,
        invCanvas,
        time,
        nSamplers,
        samplerUniforms,
        nExtras,
        shaderExtras
    );
}