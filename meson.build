# NOTE: jump to bottom for charm library and executable definitions
# NOTE: run `install_bgfx_tools.sh <BIN_DIR>` running meson, see README.md

project ('charm', ['cpp', 'c'],
         default_options : ['cpp_std=c++17',
                            'warning_level=2'])

cpp = meson.get_compiler ('cpp')

dl_dep = cpp.find_library('dl')
threads_dep = dependency('threads')

##### boost
boost_dep = dependency ('boost')

##### OpenImageIO
oiio_dep = dependency ('OpenImageIO')

#### liblo -- you know, for kids! and for OSC
lo_dep = dependency ('liblo')

##### bx
bx_include_dirs = ['bx/include', 'bx/3rdparty']
bx_sources = ['bx/src/amalgamated.cpp']

bx_lib = static_library ('bx', bx_sources,
                         include_directories : bx_include_dirs,
                         dependencies : [dl_dep, threads_dep])

bx_dep = declare_dependency (dependencies : [dl_dep, threads_dep],
                             include_directories : bx_include_dirs,
                             link_with : [bx_lib])

##### bimg
bimg_include_dirs = ['bimg/include',
                     'bimg/3rdparty',
                     'bimg/3rdparty/astc-codec',
                     'bimg/3rdparty/astc-codec/include',
                     'bimg/3rdparty/iqa/include',
                     'bimg/3rdparty/nvtt']

bimg_sources = ['bimg/src/image.cpp',
                'bimg/src/image_decode.cpp',
                'bimg/src/image_gnf.cpp',
                'bimg/src/image_cubemap_filter.cpp',
                'bimg/src/image_encode.cpp']


# blaaaaaaaaaaaaaaaaaaaaaaaaah i want to use '*' here
bimg_third_party_sources = ['bimg/3rdparty/astc/astc_averages_and_directions.cpp',
                            'bimg/3rdparty/astc/astc_block_sizes2.cpp',
                            'bimg/3rdparty/astc/astc_color_quantize.cpp',
                            'bimg/3rdparty/astc/astc_color_unquantize.cpp',
                            'bimg/3rdparty/astc/astc_compress_symbolic.cpp',
                            'bimg/3rdparty/astc/astc_compute_variance.cpp',
                            'bimg/3rdparty/astc/astc_decompress_symbolic.cpp',
                            'bimg/3rdparty/astc/astc_encoding_choice_error.cpp',
                            'bimg/3rdparty/astc/astc_find_best_partitioning.cpp',
                            'bimg/3rdparty/astc/astc_ideal_endpoints_and_weights.cpp',
                            'bimg/3rdparty/astc/astc_imageblock.cpp',
                            'bimg/3rdparty/astc/astc_integer_sequence.cpp',
                            'bimg/3rdparty/astc/astc_kmeans_partitioning.cpp',
                            'bimg/3rdparty/astc/astc_lib.cpp',
                            'bimg/3rdparty/astc/astc_partition_tables.cpp',
                            'bimg/3rdparty/astc/astc_percentile_tables.cpp',
                            'bimg/3rdparty/astc/astc_pick_best_endpoint_format.cpp',
                            'bimg/3rdparty/astc/astc_quantization.cpp',
                            'bimg/3rdparty/astc/astc_symbolic_physical.cpp',
                            'bimg/3rdparty/astc/astc_weight_align.cpp',
                            'bimg/3rdparty/astc/astc_weight_quant_xfer_tables.cpp',
                            'bimg/3rdparty/astc/mathlib.cpp',
                            'bimg/3rdparty/astc/softfloat.cpp',
                            'bimg/3rdparty/astc-codec/src/decoder/astc_file.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/codec.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/endpoint_codec.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/footprint.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/integer_sequence_codec.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/intermediate_astc_block.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/logical_astc_block.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/partition.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/physical_astc_block.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/quantization.cc',
                            'bimg/3rdparty/astc-codec/src/decoder/weight_infill.cc',
                            'bimg/3rdparty/edtaa3/edtaa3func.cpp',
                            'bimg/3rdparty/etc1/etc1.cpp',
                            'bimg/3rdparty/etc2/ProcessRGB.cpp',
                            'bimg/3rdparty/etc2/Tables.cpp',
                            'bimg/3rdparty/iqa/source/convolve.c',
                            'bimg/3rdparty/iqa/source/decimate.c',
                            'bimg/3rdparty/iqa/source/math_utils.c',
                            'bimg/3rdparty/iqa/source/ms_ssim.c',
                            'bimg/3rdparty/iqa/source/mse.c',
                            'bimg/3rdparty/iqa/source/psnr.c',
                            'bimg/3rdparty/iqa/source/ssim.c',
                            'bimg/3rdparty/libsquish/alpha.cpp',
                            'bimg/3rdparty/libsquish/clusterfit.cpp',
                            'bimg/3rdparty/libsquish/colourblock.cpp',
                            'bimg/3rdparty/libsquish/colourfit.cpp',
                            'bimg/3rdparty/libsquish/colourset.cpp',
                            'bimg/3rdparty/libsquish/maths.cpp',
                            'bimg/3rdparty/libsquish/rangefit.cpp',
                            'bimg/3rdparty/libsquish/singlecolourfit.cpp',
                            'bimg/3rdparty/libsquish/squish.cpp',
                            'bimg/3rdparty/lodepng/lodepng.cpp',
                            'bimg/3rdparty/nvtt/bc6h/zoh.cpp',
                            'bimg/3rdparty/nvtt/bc6h/zoh_utils.cpp',
                            'bimg/3rdparty/nvtt/bc6h/zohone.cpp',
                            'bimg/3rdparty/nvtt/bc6h/zohtwo.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode0.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode1.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode2.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode3.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode4.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode5.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode6.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_mode7.cpp',
                            'bimg/3rdparty/nvtt/bc7/avpcl_utils.cpp',
                            'bimg/3rdparty/nvtt/nvmath/fitting.cpp',
                            'bimg/3rdparty/nvtt/nvtt.cpp',
                            'bimg/3rdparty/pvrtc/BitScale.cpp',
                            'bimg/3rdparty/pvrtc/MortonTable.cpp',
                            'bimg/3rdparty/pvrtc/PvrTcDecoder.cpp',
                            'bimg/3rdparty/pvrtc/PvrTcEncoder.cpp',
                            'bimg/3rdparty/pvrtc/PvrTcPacket.cpp']

bimg_lib = static_library ('bimg', bimg_third_party_sources + bimg_sources,
                           include_directories : bimg_include_dirs,
                           dependencies : [bx_dep])

bimg_dep = declare_dependency (dependencies : [bx_dep],
                               include_directories : bimg_include_dirs,
                               link_with : [bimg_lib])

##### bgfx

opengl_dep = dependency ('GL')
vulkan_dep = dependency ('Vulkan')
x11_dep = dependency ('x11')

bgfx_include_dirs = ['bgfx/include',
                     'bgfx/3rdparty',
                     'bgfx/3rdparty/khronos',
                    ]
bgfx_sources = ['bgfx/src/amalgamated.cpp']

#definition for GL is gl version to use, definition for vulkan is on/off switch
bgfx_lib = static_library ('bgfx', bgfx_sources,
                           cpp_args : ['-DBGFX_CONFIG_RENDERER_OPENGL=46',
                                       '-DBGFX_CONFIG_RENDERER_VULKAN=1'],
                           include_directories : bgfx_include_dirs,
                           dependencies : [bx_dep, bimg_dep, x11_dep, opengl_dep, vulkan_dep])

bgfx_dep = declare_dependency (include_directories : bgfx_include_dirs,
                               dependencies : [bx_dep, bimg_dep, x11_dep, opengl_dep, vulkan_dep],
                               link_with : [bgfx_lib])

#### GLFW

glfw_dep = dependency ('glfw3', required : true)

##### build arguments

gst_dep       = dependency ('gstreamer-1.0')
gst_app_dep   = dependency ('gstreamer-app-1.0')
gst_video_dep = dependency ('gstreamer-video-1.0')

lib_sources = ['src/application.cpp',
               'src/Bolex.cpp',
               'src/DecodePipeline.cpp',
               'src/FlatRenderable.cpp',
               'src/FrameTime.cpp',
               'src/GeomFumble.cpp',
               'src/GraphicsApplication.cpp',
               'src/Grappler.cpp',
               'src/GrapplerPile.cpp',
               'src/interp-funcs.cpp',
               'src/IronLung.cpp',
               'src/Layer.cpp',
               'src/Matrix44.cpp',
               'src/Matte.cpp',
               'src/MatteLoader.cpp',
               'src/MattedVideoRenderable.cpp',
               'src/MotherTime.cpp',
               'src/MultiSprinkler.cpp',
               'src/Node.cpp',
               'src/PipelineTerminus.cpp',
               'src/PlatonicMaes.cpp',
               'src/RawEventParsing.cpp',
               'src/Renderable.cpp',
               'src/RoGrappler.cpp',
               'src/ProtoZoftThingGuts.cpp',
               'src/ScGrappler.cpp',
               'src/SpaceThing.cpp',
               'src/TrGrappler.cpp',
               'src/Vect.cpp',
               'src/VideoSystem.cpp',
               'src/VideoRenderable.cpp',
               'src/Zeubject.cpp',
               'src/ZeEvent.cpp',
               'src/ZESpatialEvent.cpp',
               'src/ZEYowlEvent.cpp',
               'src/ZoftThing.cpp',
               'src/bgfx_utils.cpp',
               'src/conjure-from-toml.cpp']


charm_inc = include_directories ('src')
toml_inc = include_directories ('toml11')

charm_definition_deps = [bx_dep, bimg_dep, bgfx_dep, glfw_dep,
                         gst_dep, gst_app_dep, gst_video_dep,
                         oiio_dep, boost_dep, lo_dep]

charm_lib = static_library ('charm', lib_sources,
                            include_directories : [charm_inc, toml_inc],
                            dependencies : charm_definition_deps,
                            install : true)

charm_dep = declare_dependency (include_directories : [charm_inc, toml_inc],
                                link_with : [charm_lib],
                                dependencies : charm_definition_deps)

# be sure to run `./install_bgfx_tools.sh <BIN_DIR>` before building charm
shaderc_prog = find_program ('shaderc', dirs : [get_option ('shaderc-path')])

# [type, input file, varying file, output file]
shaders = [
  # quad
  ['vertex',   'src/shaders/quad_vs.sc', 'src/shaders/quad_varying.def.sc', 'quad_vs.bin'],
  ['fragment', 'src/shaders/quad_fs.sc', 'src/shaders/quad_varying.def.sc', 'quad_fs.bin'],
  # video
  ['vertex',   'src/shaders/video_vs.sc', 'src/shaders/video_varying.def.sc', 'video_vs.bin'],
  ['fragment', 'src/shaders/video_fs.sc', 'src/shaders/video_varying.def.sc', 'video_fs.bin'],
  # matte video
  ['vertex',   'src/shaders/matte_video_vs.sc',
   'src/shaders/video_varying.def.sc', 'matte_video_vs.bin'],
  ['fragment', 'src/shaders/matte_video_fs.sc',
   'src/shaders/video_varying.def.sc', 'matte_video_fs.bin'],
]

foreach shader : shaders
  custom_target (shader[3],
                 input : [shader[1], shader[2]],
                 output : [shader[3]],
                 command : [shaderc_prog,
                            '-i', meson.source_root () + '/bgfx/src',
                            '-f', '@INPUT0@',
                            '-o', '@OUTPUT0@',
                            '--type', shader[0],
                            '--varyingdef', '@INPUT1@',
                            '--profile', '460'],
                 build_by_default : true)
endforeach

bgfx_test_exec = executable ('bgfx_test',
                             sources : ['src/bgfx_test.cpp'],
                             dependencies : [charm_dep])

dead_zone_exec = executable ('dead_zone',
                             sources : ['src/dead_zone.cpp',
                                        'src/RectangleRenderable.cpp'],
                             dependencies : [charm_dep])

config_parser_exec = executable ('config_parser',
                                 sources : ['src/config_parser.cpp'],
                                 dependencies : [charm_dep])

chud_zone_exec = executable ('chud-zone',
                             sources : ['src/chud-zone.cpp',
                                        'src/RectangleRenderable.cpp'],
                             dependencies : [charm_dep])
