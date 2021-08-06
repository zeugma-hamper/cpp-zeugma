
#ifndef BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
#define BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX


#include <base_types.hpp>
#include <class_utils.hpp>

#include <bx/bx.h>
#include <bimg/bimg.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/filepath.h>

#include <memory>
#include <string_view>
#include <vector>


namespace zeugma  {


// Shader/Program utility functions
struct ProgramResiduals
{
  bgfx::ProgramHandle program;
  bgfx::ShaderHandle vertex;
  bgfx::ShaderHandle fragment;
};

bgfx::ShaderHandle CreateShader (bx::FilePath const &_path);

ProgramResiduals CreateProgram (bx::FilePath const &_vert_path,
                                bx::FilePath const &_frag_path,
                                bool _destroy_shaders);

// Texture loading utilities
struct TextureParticulars
{
  bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
  bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Unknown;
  u32 width = 0;
  u32 height = 0;
};

//Create a texture handle and decode image to texture, texture is mutable
[[maybe_unused]]
constexpr u64 const DefaultTextureFlags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE;

TextureParticulars CreateTexture2D (bx::FilePath const &_path, u64 _bgfx_flags);
TextureParticulars CreateTexture2D (bx::FilePath const &_path, u64 _bgfx_flags, bool _create_mipmaps);

//Updates texture from image file. Format, size, etc. must match.
bgfx::TextureHandle UpdateWholeTexture2D (bgfx::TextureHandle _texture,
                                          bx::FilePath const &_path);

//Memory utility functions
bx::DefaultAllocator *GetBxDefaultAllocator ();

const bgfx::Memory *ReadFileIntoBGFX (bx::FilePath const &_path);

struct BImgFree
{
  void operator () (bimg::ImageContainer *img) const
  {
    bimg::imageFree (img);
  }
};

struct MappedFile
{
  MappedFile (std::string_view _path);
  ~MappedFile ();

  ZEUGMA_DELETE_MOVE_COPY(MappedFile);

  int fd;
  off_t size;
  void *ptr;
};

using bimg_ptr = std::unique_ptr<bimg::ImageContainer, BImgFree>;

bimg_ptr LoadKTXImage (std::string_view _path, bx::Error *_error);

// helper function to delete classes used with bgfx::makeRef
template<typename T>
void DeleteImageLeftOvers (void *, void *_user_data)
{
  T *left_overs = (T *)_user_data;
  delete left_overs;
}

//bgfx ReleaseFn is void (*) (void *ptr, void *user_data)
//this makes `free` available as ReleaseFn
void BGFXfree (void *_ptr, void *);

void BGFXFatalMessage (const char *_filePath, uint16_t _line,
                       bgfx::Fatal::Enum, const char *_str);

void BGFXDebugMessage (const char *_filePath, uint16_t _line,
                       const char *_format, va_list _argList);

struct DebugOutputCallbacks final : public bgfx::CallbackI
{
	~DebugOutputCallbacks() override {}

	void fatal (const char* _filePath , uint16_t _line,
              bgfx::Fatal::Enum _code, const char* _str) override;

	void traceVargs(const char* _filePath, uint16_t _line,
                  const char* _format, va_list _argList) override;

  void profilerBegin(const char*, uint32_t,
                     const char* , uint16_t) override {};

	void profilerBeginLiteral(const char*, uint32_t,
                            const char*, uint16_t) override
  { }

	void profilerEnd() override { }

	uint32_t cacheReadSize(uint64_t) override { return 0; }

	bool cacheRead(uint64_t, void*, uint32_t) override { return false; }
	void cacheWrite(uint64_t, const void*, uint32_t) override { }

	void screenShot(const char*, uint32_t, uint32_t,
                  uint32_t, const void*, uint32_t,
                  bool) override
  { }

	void captureBegin(uint32_t, uint32_t, uint32_t,
                    bgfx::TextureFormat::Enum, bool) override
  { }

	void captureEnd() override { }

	void captureFrame(const void*, uint32_t) override { }
};


}


#endif  //BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
