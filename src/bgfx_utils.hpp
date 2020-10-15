#ifndef BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
#define BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX

#include <base_types.hpp>

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
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

const bgfx::Memory *ReadFileIntoBGFX (bx::FilePath const &_path);

bgfx::ShaderHandle CreateShader (bx::FilePath const &_path);

ProgramResiduals CreateProgram (bx::FilePath const &_vert_path,
                                bx::FilePath const &_frag_path,
                                bool _destroy_shaders);

std::vector<bgfx::UniformHandle> GetShaderUniforms (bgfx::ShaderHandle _sh);

//Create a texture handle and decode image to texture, texture is mutable
bgfx::TextureHandle CreateTexture2D (bx::FilePath const &_path, u64 _bgfx_flags);
//Updates texture from image file. Format, size, etc. must match.
bgfx::TextureHandle UpdateWholeTexture2D (bgfx::TextureHandle _texture,
                                          bx::FilePath const &_path);

// helper function to delete classes used with bgfx::makeRef
template<typename T>
void DeleteImageLeftOvers (void *, void *_user_data)
{
  T *left_overs = (T *)_user_data;
  delete left_overs;
}

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

#endif //BGFX_IS_AWFULLY_CLOSE_TO_BUGFIX
