project ('charm', ['cpp', 'c'],
         default_options : ['cpp_std=c++17',
                            'werror=true',
                            'warning_level=2'])

cpp = meson.get_compiler ('cpp')

##### build arguments

lib_sources = ['src/SceneGraph.cpp', 'src/AnimationSystem.cpp']

charm_lib = shared_library ('charm', lib_sources,
                            dependencies : [],
                            version : '0.5',
                            install : true)

