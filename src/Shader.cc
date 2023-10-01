#include "Shader.hh"
#include "Util.hh"
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
    gl(GetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));
    char *infoLog = new char[maxLength];
    gl(GetShaderInfoLog(shader, maxLength, NULL, infoLog));
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
    gl(GetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));
    char *infoLog = new char[maxLength];
    gl(GetProgramInfoLog(program, maxLength, NULL, infoLog));
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
    gl(ShaderSource(shader, 1, &src, &length));
    gl(CompileShader(shader));
    GLint compiled = GL_FALSE;
    gl(GetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
    if (compiled != GL_TRUE) {
        char const *log = getShaderLog(shader);
        spdlog::error("Couldn't compile shader {}: {}", shader, log);
        delete[] log;
        gl(DeleteShader(shader));
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
    //gl(DeleteBuffers(1, &(vertexBuffer)));
    //if (uvBuffer) gl(DeleteBuffers(1, &(uvBuffer)));
    //if (colourBuffer) gl(DeleteBuffers(1, &(colourBuffer)));
    //delete[] textures;
}

bool Shader::Drawable::makeBuffers(GLuint *buffers) {
    gl(GenBuffers(3, buffers));
    for (int i = 0; i < 3; i++) {
        if (!buffers[i]) {
            spdlog::error("Failed to set up buffers drawable");
            if (i > 0) glDeleteBuffers(i, buffers);
            return false;
        }
    }
    return true;
}

Shader::Shader(
    GLuint vao,
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
    vao(vao),
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
    //gl(DeleteProgram(program));
    //delete[] samplers;
    //delete[] extras;
}

void Shader::update(float time) {
    gl(UseProgram(program));
    gl(Uniform1f(timeUni, time));
}

void Shader::draw(Drawable &drawable) {
    unsigned int n = drawable.predraw();
    gl(BindVertexArray(vao));
    gl(UseProgram(program));
    gl(BindBuffer(GL_ARRAY_BUFFER, drawable.vertexBuffer));
    gl(VertexAttribPointer(posAttr, 2, GL_FLOAT, false, 0, 0));
    gl(EnableVertexAttribArray(posAttr));
    if (drawable.uvBuffer) {
        gl(BindBuffer(GL_ARRAY_BUFFER, drawable.uvBuffer));
        gl(VertexAttribPointer(uvAttr, 2, GL_SHORT, false, 0, 0));
    }
    if (uvAttr != -1) gl(EnableVertexAttribArray(uvAttr));
    if (drawable.colourBuffer) {
        gl(BindBuffer(GL_ARRAY_BUFFER, drawable.colourBuffer));
        gl(VertexAttribPointer(colAttr, 4, GL_UNSIGNED_BYTE, true, 0, 0));
    }
    if (colAttr != -1) gl(EnableVertexAttribArray(colAttr));
    if (drawable.nTextures > 0) {
        for (int i = 0; i < fmin(drawable.nTextures, nSamplers); i++) {
            gl(ActiveTexture(GL_TEXTURE0 + i));
            drawable.textures[i]->bind();
            if (samplers[i].sampler != -1) {
                gl(Uniform1i(samplers[i].sampler, i));
            }
            if (samplers[i].invSize != -1) {
                gl(Uniform2f(
                    samplers[i].invSize,
                    drawable.textures[i]->invSize.x,
                    drawable.textures[i]->invSize.y
                ));
                spdlog::info("{} {} {}", drawable.vertexBuffer, drawable.textures[i]->invSize.x, drawable.textures[i]->invSize.y);
            }
        }
    }
    gl(DrawArrays(GL_TRIANGLES, 0, n));
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
        gl(DeleteShader(frag));
        return {};
    }
    GLuint program = gl(CreateProgram());
    if (!program) {
        spdlog::error("Couldn't create shader program");
        gl(DeleteShader(frag));
        gl(DeleteShader(vert));
        return {};
    }
    gl(AttachShader(program, frag));
    gl(AttachShader(program, vert));
    gl(LinkProgram(program));
    GLint linkSuccess = GL_TRUE;
    gl(GetProgramiv(program, GL_LINK_STATUS, &linkSuccess));
    if (linkSuccess != GL_TRUE) {
        char const *programLog = getProgramLog(program);
        spdlog::error("Couldn't link shader program: {}", programLog);
        delete[] programLog;
        gl(DeleteShader(frag));
        gl(DeleteShader(vert));
        gl(DeleteProgram(program));
        return {};
    }
    gl(DetachShader(program, frag));
    gl(DetachShader(program, vert));
    gl(DeleteShader(frag));
    gl(DeleteShader(vert));
    // set up vao
    GLuint vao;
    gl(GenVertexArrays(1, &vao));
    if (!vao) {
        spdlog::error("Couldn't create vao");
        gl(DeleteShader(frag));
        gl(DeleteShader(vert));
        gl(DeleteProgram(program));
        return {};
    }
    // Set up engine wide attributes and uniforms.
    gl(UseProgram(program));
    GLint position = gl(GetAttribLocation(program, "position"));
    GLint uv = gl(GetAttribLocation(program, "uv"));
    GLint colour = gl(GetAttribLocation(program, "colour"));
    GLint invCanvas = gl(GetUniformLocation(program, "canvasInv"));
    GLint time = gl(GetUniformLocation(program, "time"));
    if (invCanvas) {
        // TODO: need to get screen size here.
        gl(Uniform2f(
            invCanvas,
            0.0009765625,
            0.001302083333333
        ));
    }
    // Set up samplers and extras
    Sampler *samplerUniforms = new Sampler[nSamplers];
    for (int i = 0; i < nSamplers; i++) {
        samplerUniforms[i].sampler = gl(GetUniformLocation(program, samplers[i]));
        strcpy(invNameBuffer + INV_NAME_OFFSET, samplers[i]);
        samplerUniforms[i].invSize = gl(GetUniformLocation(
            program,
            invNameBuffer
        ));
        if (samplerUniforms[i].sampler == -1) {
            spdlog::warn("program {} lacks uniform {}", program, samplers[i]);
        }
        if (samplerUniforms[i].invSize == -1) {
            spdlog::warn("program {} lacks uniform {}", program, invNameBuffer);
        }
    }
    GLint *shaderExtras = new GLint[nExtras];
    for (int i = 0; i < nExtras; i++) {
        shaderExtras[i] = gl(GetUniformLocation(program, extras[i]));
        if (shaderExtras[i] == -1) {
            spdlog::warn("program {} lacks uniform {}", program, extras[i]);
        }
    }
    return Shader(
        vao,
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