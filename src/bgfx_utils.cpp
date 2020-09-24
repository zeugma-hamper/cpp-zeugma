#include <bgfx_utils.hpp>

#include <base_types.hpp>

#include <bgfx/platform.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bx/file.h>
#include <bx/filepath.h>

#include <OpenImageIO/imageio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

namespace charm
{

const bgfx::Memory *ReadFileIntoBGFX (bx::FilePath const &_path)
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

bgfx::ShaderHandle CreateShader (bx::FilePath const &_path)
{
  const bgfx::Memory *memory = ReadFileIntoBGFX (_path);
  if (! memory)
    return BGFX_INVALID_HANDLE;

  bgfx::ShaderHandle handle = bgfx::createShader (memory);
  if (! bgfx::isValid(handle))
    bgfx::setName (handle, _path.getFileName().getPtr(), _path.getFileName().getLength());

  return handle;
}

ProgramResiduals CreateProgram (bx::FilePath const &_vert_path,
                                bx::FilePath const &_frag_path,
                                bool _destroy_shaders)
{
  bgfx::ShaderHandle vs = CreateShader (_vert_path);
  if (! bgfx::isValid(vs))
    return {};

  bgfx::ShaderHandle fs = CreateShader (_frag_path);
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

std::vector<bgfx::UniformHandle> GetShaderUniforms (bgfx::ShaderHandle _sh)
{
  u16 uni_count = bgfx::getShaderUniforms(_sh);
  std::vector<bgfx::UniformHandle> handles{uni_count};
  bgfx::getShaderUniforms (_sh, handles.data (), uni_count);

  return handles;
}

// bgfx::TextureHandle LoadTexture2D (bx::FilePath const &_path, u64 _bgfx_flags)
// {
//   // bx::FileReader reader;
//   bx::Error error;
//   // fprintf (stderr, "image path is %s\n", _path.getCPtr());

//   // LocalMemChunk chunk = ReadWholeFile (_path);

//   // if (! reader.open (_path, &error))
//   //   return {};

//   int fd = open (_path.getCPtr(), O_RDONLY);
//   if (fd < 0)
//     return {};

//   struct stat s;
//   int status = fstat (fd, &s);
//   if (status)
//     {
//       close (fd);
//       return {};
//     }

//   void *ptr = mmap (NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
//   if (ptr == MAP_FAILED)
//     {
//       fprintf (stderr, "map failed %d\n", errno);
//       close (fd);
//       return {};
//     }

//   bx::DefaultAllocator da;
//   bimg::ImageContainer *container = bimg::imageParse (&da,
//                                                       ptr, s.st_size,
//                                                       bimg::TextureFormat::Count,
//                                                       &error);
//   munmap(ptr, s.st_size);
//   close (fd);
//   if (! container)
//     return {};

//   bgfx::Memory const *image_mem = bgfx::makeRef (container->m_data, container->m_size, release_image,
//                                                  container);



//   bgfx::TextureHandle handle = bgfx::createTexture2D(container->m_width, container->m_height,
//                                                      container->m_numMips > 1, container->m_numLayers,
//                                                      bgfx::TextureFormat::Enum (container->m_format),
//                                                      _bgfx_flags, image_mem);

//   return handle;
// }

bgfx::TextureHandle LoadTexture2D (bx::FilePath const &_path, u64 _bgfx_flags)
{
  std::unique_ptr<OIIO::ImageInput> iinput = OIIO::ImageInput::open (_path.getCPtr());
  if (! iinput)
    return {};

  OIIO::ImageSpec ispec = iinput->spec ();

  bgfx::Memory const *img_mem = bgfx::alloc(ispec.width * ispec.height * ispec.nchannels);

  iinput->read_image (OIIO::TypeDesc::UINT8, img_mem->data);

  bgfx::TextureFormat::Enum formats[5]
    { bgfx::TextureFormat::Unknown,
      bgfx::TextureFormat::R8,
      bgfx::TextureFormat::RG8,
      bgfx::TextureFormat::RGB8,
      bgfx::TextureFormat::RGBA8
    };

  bgfx::TextureHandle handle = bgfx::createTexture2D(ispec.width, ispec.height, false, 1,
                                                     formats[ispec.nchannels],
                                                     _bgfx_flags, img_mem);

  return handle;
}


}
