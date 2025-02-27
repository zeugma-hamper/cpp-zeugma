
#
# (c) treadle & loam, provisioners llc
#

# don't forget to do [git submodule update --init --recursive] from
# the top level directory (i.e. this file's) right from the git-go;
# that's how you get bx, bimg, and bgfx...

# NOTE: run `install_bgfx_tools.sh <BIN_DIR>` before running meson;
# see also README.md
# note upon note: that <BIN_DIR> should be /opt/trelopro/bin in the
# new milieu here...

# so far, either
#
# meson setup --buildtype=debug --prefix=/opt/trelopro --libdir=lib dbg
#
# or
#
# meson setup --buildtype=debug dbg
# cd dbg
# meson configure --prefix=/opt/trelopro --libdir=lib
#

project ('zeugma', ['cpp', 'c'],
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

#### toml : mkay; it's header-only, which is swell. but.
install_subdir ('toml11', install_dir : 'include')

#### nlohmann : header-only json. crisp and clean and psychotic.
install_subdir ('nlohmann', install_dir : 'include')

#### glm: you're also going to need to have this installed, but it
#### doesn't get a 'dependency' yodel here in meson because it's
#### a header-only thing...

#### here comes, then, all the bgfx gubbish

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
                         dependencies : [dl_dep, threads_dep],
                         install : true)

install_subdir ('bx/include/bx', install_dir : 'include/bx')
install_subdir ('bx/include/compat', install_dir : 'include/bx')
install_subdir ('bx/include/tinystl', install_dir : 'include/bx')
install_subdir ('bx/3rdparty', install_dir : 'include/bx')

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
                           dependencies : [bx_dep],
                           install : true)

install_subdir ('bimg/include/bimg', install_dir : 'include/bimg')
install_subdir ('bimg/3rdparty', install_dir : 'include/bimg')

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
                           dependencies : [bx_dep, bimg_dep, x11_dep, opengl_dep, vulkan_dep],
                           install : true)

# some shaders include the following, so it's gots to gits installed:
# (and, yes, it's not really a 'header', but...)

install_headers ('bgfx/src/bgfx_shader.sh', subdir: 'bgfx')

install_subdir ('bgfx/include/bgfx', install_dir : 'include/bgfx')
install_subdir ('bgfx/3rdparty', install_dir : 'include/bgfx')

bgfx_dep = declare_dependency (include_directories : bgfx_include_dirs,
                               dependencies : [bx_dep, bimg_dep, x11_dep, opengl_dep, vulkan_dep],
                               link_with : [bgfx_lib])

#### GLFW

glfw_dep = dependency ('glfw3', required : true)

##### build arguments

gst_dep       = dependency ('gstreamer-1.0')
gst_app_dep   = dependency ('gstreamer-app-1.0')
gst_video_dep = dependency ('gstreamer-video-1.0')

##### ZEUGMA #####

#
# more horror. more horror.
#

shader_prog_gulag = '/opt/trelopro/bin'

# be sure to run `./install_bgfx_tools.sh <BIN_DIR>` before building zeugma
shaderc_prog = find_program ('shaderc', dirs : shader_prog_gulag)

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
               'src/conjure-from-toml.cpp']

font_sources = ['font/cube_atlas.cpp',
                'font/font_manager.cpp',
                'font/text_buffer_manager.cpp',
                'font/text_metrics.cpp',
                'font/utf8.cpp']

zeugma_inc = include_directories ('src')
toml_inc = include_directories ('toml11')
font_inc = include_directories ('font')

zeugma_deps = [bx_dep, bimg_dep, bgfx_dep, glfw_dep,
               gst_dep, gst_app_dep, gst_video_dep,
               oiio_dep, boost_dep, lo_dep]

zeugma_lib = static_library ('Zeugma', lib_sources + font_sources + header_shader_files,
                            include_directories : [zeugma_inc, toml_inc, font_inc],
                            dependencies : zeugma_deps,
                            cpp_args : bgfx_build_args,
                            install : true)

zeugma_dep = declare_dependency (include_directories : [zeugma_inc, toml_inc, font_inc],
                                link_with : [zeugma_lib],
                                dependencies : zeugma_deps)

zeugma_headers = ['src/Alignifer.h',
                  'src/application.hpp',
                  'src/base_types.hpp',
                  'src/bgfx_utils.hpp',
                  'src/BlockTimer.hpp',
                  'src/Bolex.h',
                  'src/charm_glm.hpp',
                  'src/ch_ptr.hpp',
                  'src/class_utils.hpp',
                  'src/CMVTrefoil.h',
                  'src/CoGrappler.h',
                  'src/conjure-from-toml.h',
                  'src/DecodePipeline.hpp',
                  'src/FlatRenderable.hpp',
                  'src/FrameTime.hpp',
                  'src/Frontier.hpp',
                  'src/GeomFumble.h',
                  'src/GLFWWaterWorks.hpp',
                  'src/GraphicsApplication.hpp',
                  'src/Grappler.h',
                  'src/GrapplerPile.h',
                  'src/GridRenderable.h',
                  'src/gst_ptr.hpp',
                  'src/interp-funcs.h',
                  'src/InterpZoft.h',
                  'src/IronLung.h',
                  'src/LatchZoft.h',
                  'src/Layer.hpp',
                  'src/LinePileRenderable.h',
                  'src/LoopZoft.h',
                  'src/Matrix44.h',
                  'src/MattedVideoRenderable.hpp',
                  'src/Matte.hpp',
                  'src/MatteLoader.hpp',
                  'src/MatteLoaderPool.hpp',
                  'src/MotherTime.h',
                  'src/MultiSprinkler.hpp',
                  'src/Node.hpp',
                  'src/OmNihil.h',
                  'src/OSCWandWaterWorks.hpp',
                  'src/OverlayInstancing.hpp',
                  'src/OverlayTest.hpp',
                  'src/PipelineTerminus.hpp',
                  'src/PlatonicMaes.h',
                  'src/PolygonRenderable.h',
                  'src/ProtoZoftThingGuts.h',
                  'src/RandZoft.h',
                  'src/RawEventParsing.h',
                  'src/RectangleRenderable.hpp',
                  'src/Renderable.hpp',
                  'src/RoGrappler.h',
                  'src/ScaleZoft.h',
                  'src/ScGrappler.h',
                  'src/SinuZoft.h',
                  'src/SpaceThing.h',
                  'src/SumZoft.h',
                  'src/TampVideoTerminus.hpp',
                  'src/TextRenderable.hpp',
                  'src/TextSystem.hpp',
                  'src/TexturedRenderable.hpp',
                  'src/TrGrappler.h',
                  'src/type_int.hpp',
                  'src/Vect.h',
                  'src/vector_interop.hpp',
                  'src/vector_utils.hpp',
                  'src/VideoRenderable.hpp',
                  'src/VideoSystem.hpp',
                  'src/ZEBulletinEvent.h',
                  'src/ZeColor.h',
                  'src/ZeEvent.h',
                  'src/ZePublicWaterWorks.hpp',
                  'src/ZESpatialEvent.h',
                  'src/Zeubject.h',
                  'src/ZeUUID.h',
                  'src/ZEYowlEvent.h',
                  'src/ZoftThing.h']

font_headers = ['font/cube_atlas.h',
                'font/font_manager.h',
                'font/fs_font_basic.sc.bin.h',
                'font/text_buffer_manager.h',
                'font/text_metrics.h',
                'font/utf8.h']

install_headers (zeugma_headers, subdir : 'zeugma')
install_headers (font_headers, subdir : 'font')
