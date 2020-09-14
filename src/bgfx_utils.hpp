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

const bgfx::Memory *ReadWholeFile (bx::FilePath const &_path);

bgfx::ShaderHandle CreateShader (bx::FilePath const &_path);

ProgramResiduals CreateProgram (bx::FilePath const &_vert_path,
                                 bx::FilePath const &_frag_path,
                                 bool _destroy_shaders);

std::vector<bgfx::UniformHandle> GetShaderUniforms (bgfx::ShaderHandle _sh);

}

#endif //BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
