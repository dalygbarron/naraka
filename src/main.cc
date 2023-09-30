#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <janet.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <spdlog/spdlog.h>
#include "Texture.hh"
#include "Shader.hh"
#include "Poster.hh"
#include "Util.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

SDL_Window *window = NULL;
SDL_GLContext context;

/**
 * Should be called before any game stuff. Creates the sdl screen and
 * initialises SDL and opengl and glew and all of that shit.
 * @return true if all went well and false if you should just call the deinit
 *         function and gtfo.
 */
bool init() {
    // Init sdl.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Couldn't initialise. SDL: {}", SDL_GetError());
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        spdlog::error("Couldn't initialise. SDL_Image: {}", IMG_GetError());
    }
    // set opengl version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );
    // create window
    window = SDL_CreateWindow(
        "Gamer Momento",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!window) {
        spdlog::error("Couldn't create window. SDL: {}", SDL_GetError());
        return false;
    }
    // create gl context.
    context = SDL_GL_CreateContext(window);
    if (!context) {
        spdlog::error("Couldn't create gl context. SDL: {}", SDL_GetError());
        return false;
    }
    // initialise glew for some reason. Dunno what it does.
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        spdlog::error(
            "Couldn't init glew: {}",
            (char const *)glewGetErrorString(glewError)
        );
        return false;
    }
    // turn on vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        spdlog::warn("Couldn't setup vsync. SDL: {}", SDL_GetError());
    }
    return true;
}

/**
 * Deinitialises the window and graphics system.
 */
void close() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * Entry point to the program.
 * @param argc number of commandline args.
 * @param argv commandline args.
 * @return status code always 0 because I don't care about it.
 */
int main(int argc, char const **argv) {
    if (!init()) goto end;
    {
        std::optional<Texture> tex = Texture::create("pic.png");
        if (!tex) {
            spdlog::error("texture load fails");
            goto end;
        }
        
        std::optional<Shader> shader = Shader::create();
        if (!shader) {
            spdlog::error("shader load fails");
            goto end;
        }
        Texture *textures = &(*tex);
        std::optional<Poster> poster = Poster::create(
            Util::Rect(10, 10, 100, 200),
            Util::Rect(10, 10, 100, 100),
            Util::WHITE,
            1,
            &textures
        );
        if (!poster) {
            spdlog::error("Couldn't make poster");
            goto end;
        }
        glClearColor(0.f, 0.1f, 0.2f, 1.f);
        for (int i = 0; i < 5000; i++) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) goto end;
            }
            GLenum err;
            while((err = glGetError()) != GL_NO_ERROR) {
                spdlog::error("gl error 0x{:x}", err);
            }
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            shader->draw(*poster);
            SDL_GL_SwapWindow(window);
        }
    }
    end:
        close();
        spdlog::info("bye bye");
        return 0;
}