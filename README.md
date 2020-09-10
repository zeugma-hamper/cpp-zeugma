## dead_zone

this is the video player i've been working on. i've been dealing with some shader problems. see below.

## shaders

there are some weird bits to bgfx's shader preprocessing / compiling. i'm going to expand this list as i discover new things.

- use `void main()`: it bails if you put a space as in `void main ()`
- without specifying profile glsl version, it defaults to 1.2. 
- all examples use `gl_FragColor` or `gl_FragData[n]`, but this causes problems on newer intel GL context versions. use `layout (location = 0) out vec4 out_color;` instead. see `quad_fs.sc` and `video_fs.sc` for examples.
- it's possible for shaderc to compile successfully and the GL implementation can still barf. bgfx sucks at reporting the actual error, but using gdb to stop at the shader error `assert`, you can go up four or so frames and `print log`. 

## building with shaderc
i haven't automated building shaders yet, so here is the command i
used from the root of the project directory:

    bld/shaderc -i bgfx/src -f src/shaders/quad_vs.sc -o bld/quad_vs.bin --type vertex --varyingdef src/quad_varying.def.sc --profile 460
    bld/shaderc -i bgfx/src -f src/shaders/quad_fs.sc -o bld/quad_fs.bin --type fragment --varyingdef src/shaders/quad_varying.def.sc --profile 460

where:

    -i: include directory
    -f: shader file
    -o: output file
    --varyingdef: file defining inputs and outputs from shaders
    --platform: which shader language to compile. to specify GLSL version,
      use version number with decimal shifted right two places.

i made `compile_shader.sh`; see the top of the file for argument order.

## shader naming scheme

i'm adopting the naming scheme `name_type.sc`, where `type` is `vertex`, `fragment`, or `varying.def` and `name` is whatever identifier you want. this has the benefit of keeping related shaders together in directory listings.
