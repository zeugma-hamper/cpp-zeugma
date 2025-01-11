
# comprehensive build instructions arriving shortly

It's true!

# linux build notes (adapted) from @carltonj, c. 2021

Zeugma test on Ubuntu:

1. Create an `/opt/trelopro/bin` directory and make it owned by you and/or writeable by all
2. Clone the zeugma repo: ~~ https://gitlab.com/vanderoops/zeugma-wip ~~  https://gitlab.com/zeugma-hamper/cpp-zeugma  ; cd to that directory
3. do `git submodule update —init —recursive`
4. run `./install_bgfx_tools.sh /opt/trelopro/bin`
5. Install dependencies.  These will include, but may not be limited to:

```
libboost-all-dev
cmake
ninja
libopenimageio-dev
liblo-dev
libglu1-mesa-dev
libvulkan-dev
libglfw3-dev
libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
libglm-dev
```

6. Follow the instructions in meson.build; then, making sure you're sitting in the build directory (`<zeugma-top-level>/dbg`, typically), run `ninja`.
7. Once ninja has run successfully, do `ninja install` to populate:

`/opt/trelopro/include` *(and)* `/opt/trelopro/lib`

8. Switch to playpen directory, build, ninja, run


# original notes

immediately following are the notes, authored by @lintwaddle, that originally
accompanied the codebase (which, at the time, consisted of zeugma intermingled
with *tamper 2.0*, the application that first used zeugma).

### dead_zone

this is the video player i've been working on. i've been dealing with some shader problems. see below.

### shaders

there are some weird bits to bgfx's shader preprocessing / compiling. i'm going to expand this list as i discover new things.

- use `void main()`: it bails if you put a space as in `void main ()`
- without specifying profile glsl version, it defaults to 1.2.
- all examples use `gl_FragColor` or `gl_FragData[n]`, but this causes problems on newer intel GL context versions (skull canyon, NUC). use `layout (location = 0) out vec4 out_color;` in the fragment shader instead. see `quad_fs.sc` and `video_fs.sc` for examples.
- it's possible for shaderc to compile successfully and the GL implementation can still barf. bgfx sucks at reporting the actual error, but using gdb to stop at the shader error `assert`, you can go up four or so frames and `print log`. 

### building with shaderc
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


### shader naming scheme

i'm adopting the naming scheme `name_type.sc`, where `type` is `vs`, `fs`, or `varying.def` and `name` is whatever identifier you want. this has the benefit of keeping related shaders together in directory listings.
