#ifndef BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
#define BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX

#include <bgfx/bgfx.h>
#include <bx/filepath.h>

#include <vector>

namespace charm
{

const bgfx::Memory *read_whole_file (bx::FilePath const &_path);

bgfx::ShaderHandle create_shader (bx::FilePath const &_path);

std::vector<bgfx::UniformHandle> get_shader_uniforms (bgfx::ShaderHandle _sh);

}

#endif //BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
