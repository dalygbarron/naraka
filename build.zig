const std = @import("std");
const Sdk = @import("Sdk.zig"); // Import the Sdk at build time

pub fn build(b: *std.Build.Builder) !void {
    // Determine compilation target
    const target = b.standardTargetOptions(.{});

    // Create a new instance of the SDL2 Sdk
    const sdk = Sdk.init(b, null);

    // Create executable for our example
    const naraka = b.addExecutable(.{
        .name = "naraka",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
    });
    sdk.link(naraka, .dynamic); // link SDL2 as a shared library

    // Add "sdl2" package that exposes the SDL2 api (like SDL_Init or SDL_CreateWindow)
    naraka.addModule("sdl2", sdk.getNativeModule());

    // Install the executable into the prefix when invoking "zig build"
    b.installArtifact(naraka);
}
