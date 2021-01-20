# NOTE: jump to bottom for charm library and executable definitions
# NOTE: run `install_bgfx_tools.sh <BIN_DIR>` running meson, see README.md

project ('charm', ['cpp', 'c'],
         default_options : ['cpp_std=c++17',
                            'warning_level=2'])

cpp = meson.get_compiler ('cpp')

dl_dep = cpp.find_library('dl')
threads_dep = dependency('threads')

##### boost
boost_dep = dependency ('boost', modules : ['program_options'])

##### OpenImageIO
oiio_dep = dependency ('OpenImageIO')

#### liblo -- you know, for kids! and also for OSC.
lo_dep = dependency ('liblo')

#### libGLU: sure, it's deprecated; but then: aren't we all?
glu_dep = cpp.find_library ('libGLU')

bgfx_build_args = []
if (get_option('bgfx-msgs').enabled ())
  bgfx_build_args = bgfx_build_args + ['-DBX_CONFIG_DEBUG=1', '-DBGFX_CONFIG_DEBUG=1']
endif

##### bx
bx_include_dirs = ['bx/include', 'bx/3rdparty']
bx_sources = ['bx/src/amalgamated.cpp']

bx_lib = static_library ('bx', bx_sources,
                         cpp_args : bgfx_build_args,
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
                           cpp_args : bgfx_build_args,
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
bgfx_lib_cpp_args = bgfx_build_args + ['-DBGFX_CONFIG_RENDERER_OPENGL=46', '-DBGFX_CONFIG_RENDERER_VULKAN=1']
bgfx_lib = static_library ('bgfx', bgfx_sources,
                           cpp_args : bgfx_lib_cpp_args,
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

##### CHARM #####

# be sure to run `./install_bgfx_tools.sh <BIN_DIR>` before building charm
shaderc_prog = find_program ('shaderc', dirs : [get_option ('shaderc-path')])

# NOTE: this is only for shaders that turn into headers, most go below
header_shaders = [
  ['vertex',   'font/vs_font_basic.sc',
               'font/varying.def.sc',
               'vs_font_basic.bin.h'],
  ['fragment', 'font/fs_font_basic.sc',
               'font/varying.def.sc',
               'fs_font_basic.bin.h'],
  ['vertex',   'font/vs_font_distance_field.sc',
               'font/varying.def.sc',
               'vs_font_distance_field.bin.h'],
  ['fragment', 'font/fs_font_distance_field.sc',
               'font/varying.def.sc',
               'fs_font_distance_field.bin.h'],
  ['vertex',   'font/vs_font_distance_field_subpixel.sc',
               'font/varying.def.sc',
               'vs_font_distance_field_subpixel.bin.h'],
  ['fragment', 'font/fs_font_distance_field_subpixel.sc',
               'font/varying.def.sc',
               'fs_font_distance_field_subpixel.bin.h'],
]

header_shader_files = []

foreach shader : header_shaders
  fl = custom_target (shader[3],
                      input : [shader[1], shader[2]],
                      output : [shader[3]],
                      command : [shaderc_prog,
                                 '-i', meson.source_root () + '/bgfx/src',
                                 '-i', meson.source_root () + '/src/shaders/include',
                                 '-i', meson.source_root () + '/bgfx/examples',
                                 '-f', '@INPUT0@',
                                 '-o', '@OUTPUT0@',
                                 '--bin2c',
                                 '--type', shader[0],
                                 '--varyingdef', '@INPUT1@',
                                 '--profile', '460'],
                      build_by_default : true)
  header_shader_files += [fl]
endforeach

lib_sources = ['src/application.cpp',
               'src/Alignifer.cpp',
               'src/AudioMessenger.cpp',
               'src/Bolex.cpp',
               'src/CoGrappler.cpp',
               'src/CMVTrefoil.cpp',
               'src/DecodePipeline.cpp',
               'src/FlatRenderable.cpp',
               'src/FrameTime.cpp',
               'src/Frontier.cpp',
               'src/GeomFumble.cpp',
               'src/GLFWWaterWorks.cpp',
               'src/GraphicsApplication.cpp',
               'src/Grappler.cpp',
               'src/GrapplerPile.cpp',
               'src/GridRenderable.cpp',
               'src/interp-funcs.cpp',
               'src/IronLung.cpp',
               'src/Layer.cpp',
               'src/LinePileRenderable.cpp',
               'src/Matrix44.cpp',
               'src/Matte.cpp',
               'src/MatteLoader.cpp',
               'src/MatteLoaderPool.cpp',
               'src/MattedVideoRenderable.cpp',
               'src/MotherTime.cpp',
               'src/MultiSprinkler.cpp',
               'src/Node.cpp',
               'src/OSCWandWaterWorks.cpp',
               'src/OverlayTest.cpp',
               'src/OverlayInstancing.cpp',
               'src/PipelineTerminus.cpp',
               'src/PlatonicMaes.cpp',
               'src/PolygonRenderable.cpp',
               'src/RandZoft.cpp',
               'src/RawEventParsing.cpp',
               'src/RectangleRenderable.cpp',
               'src/Renderable.cpp',
               'src/RoGrappler.cpp',
               'src/ProtoZoftThingGuts.cpp',
               'src/ScGrappler.cpp',
               'src/SpaceThing.cpp',
               'src/TampVideoTerminus.cpp',
               'src/TextRenderable.cpp',
               'src/TextSystem.cpp',
               'src/TexturedRenderable.cpp',
               'src/TrGrappler.cpp',
               'src/Vect.cpp',
               'src/VideoSystem.cpp',
               'src/VideoRenderable.cpp',
               'src/Zeubject.cpp',
               'src/ZeColor.cpp',
               'src/ZeEvent.cpp',
               'src/ZePublicWaterWorks.cpp',
               'src/ZeUUID.cpp',
               'src/ZEBulletinEvent.cpp',
               'src/ZESpatialEvent.cpp',
               'src/ZEYowlEvent.cpp',
               'src/ZoftThing.cpp',
               'src/base_types.cpp',
               'src/bgfx_utils.cpp',
               'src/conjure-from-toml.cpp',
               'src/global-params.cpp']

font_sources = ['font/cube_atlas.cpp',
                'font/font_manager.cpp',
                'font/text_buffer_manager.cpp',
                'font/text_metrics.cpp',
                'font/utf8.cpp']

charm_inc = include_directories ('src')
toml_inc = include_directories ('toml11')
font_inc = include_directories ('font')

charm_definition_deps = [bx_dep, bimg_dep, bgfx_dep, glfw_dep,
                         gst_dep, gst_app_dep, gst_video_dep,
                         oiio_dep, boost_dep, lo_dep]

charm_lib = static_library ('charm', lib_sources + font_sources + header_shader_files,
                            include_directories : [charm_inc, toml_inc, font_inc],
                            dependencies : charm_definition_deps,
                            cpp_args : bgfx_build_args,
                            install : true)

charm_dep = declare_dependency (include_directories : [charm_inc, toml_inc, font_inc],
                                link_with : [charm_lib],
                                dependencies : charm_definition_deps)


##### SHADERS #####

# [type, input file, varying file, output file]
shaders = [
  # the very most basicest
  ['vertex',   'src/shaders/primal_vs.sc',
               'src/shaders/primal_varying.def.sc',
                'primal_vs.bin'],
  ['fragment', 'src/shaders/primal_fs.sc',
               'src/shaders/primal_varying.def.sc',
                'primal_fs.bin'],
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
  # collage background for demo
  ['vertex',   'src/shaders/collage_bg_vs.sc',
   'src/shaders/collage_bg_varying.def.sc', 'collage_bg_vs.bin'],
  ['fragment', 'src/shaders/collage_bg_fs.sc',
   'src/shaders/collage_bg_varying.def.sc', 'collage_bg_fs.bin'],
  # tex-quaddish object
  ['vertex',   'src/shaders/textured_renderable.vs.sc',
   'src/shaders/basic_varying.def.sc', 'textured_renderable.vs.bin'],
  ['fragment', 'src/shaders/textured_renderable.fs.sc',
   'src/shaders/basic_varying.def.sc', 'textured_renderable.fs.bin'],
  # gridularness
  ['vertex',   'src/shaders/grid_vs.sc',
   'src/shaders/grid_varying.def.sc', 'grid_vs.bin'],
  ['fragment', 'src/shaders/grid_fs.sc',
   'src/shaders/grid_varying.def.sc', 'grid_fs.bin'],
  # overlay testing
  ['vertex',   'src/shaders/overlay_vs.sc',
   'src/shaders/overlay_varying.def.sc', 'overlay_vs.bin'],
  ['fragment', 'src/shaders/overlay_fs.sc',
   'src/shaders/overlay_varying.def.sc', 'overlay_fs.bin'],
  # overlay testing
  ['vertex',   'src/shaders/overlay_instance_vs.sc',
   'src/shaders/overlay_varying.def.sc', 'overlay_instance_vs.bin'],
  ['fragment', 'src/shaders/overlay_instance_fs.sc',
   'src/shaders/overlay_varying.def.sc', 'overlay_instance_fs.bin'],
]

foreach shader : shaders
  custom_target (shader[3],
                 input : [shader[1], shader[2]],
                 output : [shader[3]],
                 command : [shaderc_prog,
                            '-i', meson.source_root () + '/bgfx/src',
                            '-i', meson.source_root () + '/src/shaders/include',
                            '-f', '@INPUT0@',
                            '-o', '@OUTPUT0@',
                            '--type', shader[0],
                            '--varyingdef', '@INPUT1@',
                            '--profile', '460'],
                 build_by_default : true)
endforeach



##### EXECUTABLES #####
bgfx_test_exec = executable ('bgfx_test',
                             sources : ['src/bgfx_test.cpp'],
                             dependencies : [charm_dep])

dead_zone_exec = executable ('dead_zone',
                             sources : ['src/dead_zone.cpp'],
                             dependencies : [charm_dep])

text_zone_exec = executable ('text_zone',
                             sources : ['src/text_zone.cpp'],
                             dependencies : [charm_dep])

overlay_zone_exec = executable ('overlay_zone',
                                sources : ['src/overlay_zone.cpp'],
                                dependencies : [charm_dep])

config_parser_exec = executable ('config_parser',
                                 sources : ['src/config_parser.cpp'],
                                 dependencies : [charm_dep])

chud_zone_exec = executable ('chud-zone',
                             sources : ['src/chud-zone.cpp'],
                             dependencies : [charm_dep])

tri_demo_exec = executable ('tri_demo',
                            sources : ['src/tri_demo.cpp',
                                       'src/Collage.cpp',
                                       'src/TriBand.cpp'],
                            dependencies : [charm_dep, glu_dep])

tample_exec = executable ('tample',
                           sources : ['src/AtomicFreezone.cpp',
                                      'src/GraumanPalace.cpp',
                                      'src/Jigglegon.cpp',
                                      'src/OeuvreAfterlife.cpp',
                                      'src/Orksur.cpp',
                                      'src/SilverScreen.cpp',
                                      'src/SonoChoosist.cpp',
                                      'src/SpectacleCauseway.cpp',
                                      'src/Ticato.cpp',
                                      'src/Timeline.cpp',
                                      'src/gallimaufry.cpp',
                                      'src/tamparams.cpp',
                                      'src/tample.cpp'
                                     ],
                           dependencies : [charm_dep, glu_dep])

scan_mattes_exec = executable ('scan_mattes',
                               sources : ['src/scan_mattes.cpp'],
                               dependencies : [charm_dep])

audio_runthrough_exec = executable ('audio_runthrough',
                                    sources : ['src/audio_runthrough.cpp'],
                                    dependencies : [charm_dep])
