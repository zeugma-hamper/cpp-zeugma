
#ifndef DISTURBING_YOUR_DRIVING
#define DISTURBING_YOUR_DRIVING


#include <class_utils.hpp>

#include <font_manager.h>
#include <text_buffer_manager.h>

#include <filesystem>
#include <string_view>
#include <unordered_map>


namespace zeugma  {


class TextSystem
{
 public:

  ~TextSystem ();

  ZEUGMA_DELETE_MOVE_COPY(TextSystem);

  static bool Initialize ();
  static void ShutDown ();

  static TextSystem *GetSystem ();
  static TextBufferManager *GetTextBufferManager ();
  static FontManager *GetFontManager ();

  // don't worry about deallocating any of these handles. unless
  // resource usage becomes a pressing issue, i'm going to hold on to
  // everything internally and free at shutdown.
  TrueTypeHandle LoadTTF (u8 const *_buffer, u32 _size, std::string_view _name);
  TrueTypeHandle LoadTTF (std::filesystem::path const &_path, std::string_view _name);
  TrueTypeHandle FindTTF (std::string_view _name) const;
  TrueTypeHandle FindTTF (std::filesystem::path const &_path) const;

  FontHandle CreateFontByPixelSize (TrueTypeHandle _handle, u32 _face_index,
                                    u32 _pixel_size, std::string_view _name);
  FontHandle CreateFontByPixelSize (TrueTypeHandle _handle, u32 _face_index,
                                    u32 _pixel_size, u32 _font_type,
                                    std::string_view _name);
  FontHandle FindFont (std::string_view _name) const;

  bgfx::UniformHandle GetAdjColorUniform () const;
  bgfx::UniformHandle GetDimensionsUniform () const;
  bgfx::UniformHandle GetOverUniform () const;
  bgfx::UniformHandle GetUpUniform () const;

 protected:
  TextSystem ();

  bgfx::UniformHandle m_uni_adj_color;
  bgfx::UniformHandle m_uni_dimensions;
  bgfx::UniformHandle m_uni_over;
  bgfx::UniformHandle m_uni_up;

  FontManager *m_font_manager;
  TextBufferManager *m_text_buffer_manager;

  std::wstring m_preload_set;
  std::unordered_map<std::string, TrueTypeHandle> m_ttfs_path;
  std::unordered_map<std::string, TrueTypeHandle> m_ttfs;
  std::unordered_map<std::string, FontHandle> m_fonts;
};


}


#endif  // DISTURBING_YOUR_DRIVING
