project('Naraka', 'cpp', default_options: ['c_std=c11', 'cpp_std=c++20'])
sources = [
  'src/main.cc',
  'src/Texture.cc',
  'src/Shader.cc',
  'src/Poster.cc',
  'src/Batch.cc',
  'src/Util.cc'
]
deps = [
  dependency('janet'),
  dependency('spdlog'),
  dependency('gl'),
  dependency('glew'),
  dependency('sdl2'),
  dependency('sdl2_image'),
  dependency('glm')
]
main = executable(
  'main',
  sources: sources,
  dependencies: deps
)
