#include <bgfx_utils.hpp>

#include <base_types.hpp>

#include <bgfx/platform.h>
#include <bx/file.h>
#include <bx/filepath.h>

namespace charm
{

const bgfx::Memory *read_whole_file (bx::FilePath const &_path)
{
  bx::Error error;
  bx::FileReader reader;
  if (! reader.open (_path, &error))
    return nullptr;

  i64 size = reader.seek (0, bx::Whence::End);
  const bgfx::Memory *mem = bgfx::alloc (size);
  reader.seek (0, bx::Whence::Begin);

  if (0 >= reader.read (mem->data, mem->size, &error))
    {
      reader.close ();
      return nullptr;
    }

  reader.close ();
  return mem;
}

bgfx::ShaderHandle create_shader (bx::FilePath const &_path)
{
  const bgfx::Memory *memory = read_whole_file (_path);
  if (! memory)
    return BGFX_INVALID_HANDLE;

  bgfx::ShaderHandle handle = bgfx::createShader (memory);
  if (! bgfx::isValid(handle))
    bgfx::setName (handle, _path.getFileName().getPtr(), _path.getFileName().getLength());

  return handle;
}

ProgramResiduals create_program (bx::FilePath const &_vert_path,
                                 bx::FilePath const &_frag_path,
                                 bool _destroy_shaders)
{
  bgfx::ShaderHandle vs = create_shader (_vert_path);
  if (! bgfx::isValid(vs))
    return {};

  bgfx::ShaderHandle fs = create_shader (_frag_path);
  if (! bgfx::isValid(fs))
    {
      bgfx::destroy (vs);
      return {};
    }

  bgfx::ProgramHandle prog = bgfx::createProgram(vs, fs, _destroy_shaders);
  if (! bgfx::isValid(prog))
    {
      bgfx::destroy (vs);
      bgfx::destroy (fs);
      return {};
    }

  if (_destroy_shaders)
    return {prog, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE};
  else
    return {prog, vs, fs};
}

std::vector<bgfx::UniformHandle> get_shader_uniforms (bgfx::ShaderHandle _sh)
{
  u16 uni_count = bgfx::getShaderUniforms(_sh);
  std::vector<bgfx::UniformHandle> handles{uni_count};
  bgfx::getShaderUniforms (_sh, handles.data (), uni_count);

  return handles;
}

}
