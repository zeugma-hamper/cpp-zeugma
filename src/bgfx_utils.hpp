#ifndef BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
#define BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX

#include <bgfx/bgfx.h>
#include <bx/filepath.h>

#include <vector>

namespace charm
{
struct ProgramResiduals
{
  bgfx::ProgramHandle program;
  bgfx::ShaderHandle vertex;
  bgfx::ShaderHandle fragment;
};

const bgfx::Memory *read_whole_file (bx::FilePath const &_path);

bgfx::ShaderHandle create_shader (bx::FilePath const &_path);

ProgramResiduals create_program (bx::FilePath const &_vert_path,
                                 bx::FilePath const &_frag_path,
                                 bool _destroy_shaders);

std::vector<bgfx::UniformHandle> get_shader_uniforms (bgfx::ShaderHandle _sh);

}

#endif //BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
