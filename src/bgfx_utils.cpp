#include <bgfx_utils.hpp>

#include <base_types.hpp>

#include <BlockTimer.hpp>

#include <bgfx/platform.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bx/file.h>
#include <bx/filepath.h>

#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
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

bx::DefaultAllocator *GetBxDefaultAllocator ()
{
  static bx::DefaultAllocator *alloc = new bx::DefaultAllocator;
  return alloc;
}

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

TextureParticulars CreateTexture2D (bx::FilePath const &_path, u64 _bgfx_flags)
{
  return CreateTexture2D(_path, _bgfx_flags, false);
}

TextureParticulars CreateTexture2D (bx::FilePath const &_path, u64 _bgfx_flags, bool _create_mipmaps)
{
  OIIO::ImageBuf image (_path.getCPtr());
  if (! image.initialized())
    return {};

  OIIO::ImageSpec ispec = image.spec ();

  bgfx::TextureFormat::Enum formats[5]
    { bgfx::TextureFormat::Unknown,
      bgfx::TextureFormat::R8,
      bgfx::TextureFormat::RG8,
      bgfx::TextureFormat::RGB8,
      bgfx::TextureFormat::RGBA8
    };

  bgfx::TextureInfo tinfo;
  bgfx::calcTextureSize(tinfo, ispec.width, ispec.height, 1, false,
                        _create_mipmaps, 1, formats[ispec.nchannels]);

  bgfx::Memory const *img_mem = bgfx::alloc(tinfo.storageSize);

  // in bgfx, if texture is created with valid memory pointer, then the texture is immutable.
  // here, create with null pointer, then upload so texture is mutable.
  bgfx::TextureHandle handle = bgfx::createTexture2D(ispec.width, ispec.height,
                                                     _create_mipmaps, 1,
                                                     formats[ispec.nchannels],
                                                     _bgfx_flags, nullptr);

  ispec.set_format(OIIO::TypeDesc::UINT8);
  OIIO::ImageBuf output (ispec, img_mem->data);
  output.copy_pixels (image);

  if (_create_mipmaps)
    {
      u8 *data_ptr = img_mem->data + (ispec.width * ispec.height * ispec.nchannels);
      for (u32 i = 1; i < tinfo.numMips; ++i)
        {
          OIIO::ROI roi (0, std::max (ispec.roi().xend/2, 1),
                         0, std::max (ispec.roi().yend/2, 1));
          ispec.set_roi (roi);
          OIIO::ImageBuf mip (ispec, data_ptr);
          if (! OIIO::ImageBufAlgo::resize (mip, image, "", 0.0f, roi))
            fprintf (stderr, "failed to create mip %u\n", i);
          data_ptr += (ispec.width * ispec.height * ispec.nchannels);
        }
    }

  bgfx::updateTexture2D(handle, 0, 0, 0, 0, ispec.width, ispec.height, img_mem);

  return {handle, formats[ispec.nchannels], u32 (ispec.width), u32 (ispec.height)};
}

bgfx::TextureHandle UpdateWholeTexture2D (bgfx::TextureHandle _texture,
                                          bx::FilePath const &_path)
{
  std::unique_ptr<OIIO::ImageInput> iinput = OIIO::ImageInput::open (_path.getCPtr());
  if (! iinput)
    return {};

  OIIO::ImageSpec const &ispec = iinput->spec ();

  bgfx::Memory const *img_mem = bgfx::alloc(ispec.width * ispec.height * ispec.nchannels);

  iinput->read_image (OIIO::TypeDesc::UINT8, img_mem->data);

  bgfx::updateTexture2D (_texture, 0, 0, 0, 0, ispec.width, ispec.height, img_mem);

  return _texture;
}

struct MappedFile
{
  MappedFile (std::string_view _path)
    : fd {-1},
      size {0},
      ptr {nullptr}
  {
    fd = open (_path.data (), O_RDONLY | O_CLOEXEC);
    if (fd <= 0)
      return;

    size = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);

    ptr = mmap (NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  }

  ~MappedFile ()
  {
    if (ptr)
      munmap (ptr, size);
    ptr = nullptr;

    if (fd > 0)
      close (fd);
    fd = 0;
  }

  int fd;
  off_t size;
  void *ptr;
};

bimg_ptr LoadKTXImage (std::string_view _path, bx::Error *_error)
{
  MappedFile fl {_path};
  if (fl.ptr == nullptr)
    return {};

  return bimg_ptr {bimg::imageParseKtx (GetBxDefaultAllocator(), fl.ptr, fl.size, _error)};
}

bimg_ptr LoadBImage (std::string_view _path, bx::Error *_error)
{
  MappedFile fl {_path};
  if (fl.ptr == nullptr)
    return {};

  return bimg_ptr {bimg::imageParseKtx (GetBxDefaultAllocator(), fl.ptr, fl.size, _error)};
}

void BGFXfree (void *_ptr, void *)
{
  free (_ptr);
}

void BGFXFatalMessage (const char *_filePath, uint16_t _line, bgfx::Fatal::Enum, const char *_str)
{
  fprintf (stderr, "fatal error at %s:%u\n%s\n", _filePath, _line, _str);
}

void BGFXDebugMessage (const char *_filePath, uint16_t _line,
                       const char *_format, va_list _argList)
{
  fprintf (stderr, "debug at %s:%u\n", _filePath, _line);
  vfprintf (stderr, _format, _argList);
}

void DebugOutputCallbacks::fatal (const char* _filePath , uint16_t _line,
                                  bgfx::Fatal::Enum _code, const char* _str)
{
  BGFXFatalMessage(_filePath, _line, _code, _str);
}

void DebugOutputCallbacks::traceVargs(const char* _filePath, uint16_t _line,
                                      const char* _format, va_list _argList)
{
  BGFXDebugMessage(_filePath, _line, _format, _argList);
}


}
