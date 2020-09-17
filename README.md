## dead_zone

this is the video player i've been working on. i've been dealing with some shader problems. see below.

## shaders

there are some weird bits to bgfx's shader preprocessing / compiling. i'm going to expand this list as i discover new things.

- use `void main()`: it bails if you put a space as in `void main ()`
- without specifying profile glsl version, it defaults to 1.2. 
- all examples use `gl_FragColor` or `gl_FragData[n]`, but this causes problems on newer intel GL context versions. use `layout (location = 0) out vec4 out_color;` instead. see `quad_fs.sc` and `video_fs.sc` for examples.
- it's possible for shaderc to compile successfully and the GL implementation can still barf. bgfx sucks at reporting the actual error, but using gdb to stop at the shader error `assert`, you can go up four or so frames and `print log`. 

## building with shaderc
first, install bgfx's tools with the `install_bgfx_tools.sh` script. it expects one argument: the binary directory to install. it should be in your path or added to your path so meson can find `shaderc`. any new shaders you create should be added to `meson.build` similar to the below array:

    # [type, input file, varying file, output file]
    shaders = [
      # video
      ['vertex',   'src/shaders/video_vs.sc', 'src/shaders/video_varying.def.sc', 'video_vs.bin'],
      ['fragment', 'src/shaders/video_fs.sc', 'src/shaders/video_varying.def.sc', 'video_fs.bin'],
      # quad
      ['vertex',   'src/shaders/quad_vs.sc', 'src/shaders/quad_varying.def.sc', 'quad_vs.bin'],
      ['fragment', 'src/shaders/quad_fs.sc', 'src/shaders/quad_varying.def.sc', 'quad_fs.bin'],
    ]


## shader naming scheme

i'm adopting the naming scheme `name_type.sc`, where `type` is `vertex`, `fragment`, or `varying.def` and `name` is whatever identifier you want. this has the benefit of keeping related shaders together in directory listings.
