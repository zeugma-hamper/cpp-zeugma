
#include <TextSystem.hpp>

#include <font_manager.h>
#include <text_buffer_manager.h>

#include <bgfx_utils.hpp>


namespace zeugma  {


static TextSystem *s_text_system = nullptr;

TextSystem::TextSystem ()
  :  m_uni_adj_color (bgfx::createUniform("u_adj_color", bgfx::UniformType::Vec4)),
     m_uni_dimensions (bgfx::createUniform("u_dimensions", bgfx::UniformType::Vec4)),
     m_uni_over (bgfx::createUniform("u_over", bgfx::UniformType::Vec4)),
     m_uni_up (bgfx::createUniform("u_up", bgfx::UniformType::Vec4)),
     m_font_manager (new FontManager (4096)),
     m_text_buffer_manager (new TextBufferManager (m_font_manager)),
     m_preload_set (L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                   ",./<>?;':\"[]{}\\|`~!@#$%^&*()_-=+")
{ }

TextSystem::~TextSystem ()
{
  delete m_text_buffer_manager;
  m_text_buffer_manager = nullptr;

  for (auto &[s, f] : m_fonts)
    m_font_manager->destroyFont (f);
  m_fonts.clear();

  for (auto &[s, f] : m_ttfs)
    m_font_manager->destroyTtf (f);
  m_ttfs.clear ();
  m_ttfs_path.clear ();

  delete m_font_manager;
  m_font_manager = nullptr;

  bgfx::destroy (m_uni_adj_color);
  bgfx::destroy (m_uni_dimensions);
  bgfx::destroy (m_uni_over);
  bgfx::destroy (m_uni_up);
}

bool TextSystem::Initialize ()
{
  auto *ts = new TextSystem;
  s_text_system = ts;

  return (bool)ts;
}

void TextSystem::ShutDown ()
{
  auto *ts = s_text_system;
  s_text_system = nullptr;

  delete ts;
}

TextSystem *TextSystem::GetSystem ()
{
  return s_text_system;
}

TextBufferManager *TextSystem::GetTextBufferManager ()
{
  if (auto *ts = GetSystem(); ts)
    return ts->m_text_buffer_manager;

  return nullptr;
}

FontManager *TextSystem::GetFontManager ()
{
  if (auto *ts = GetSystem(); ts)
    return ts->m_font_manager;

  return nullptr;
}

TrueTypeHandle TextSystem::LoadTTF (u8 const *_buffer, u32 _size, std::string_view _name)
{
  TrueTypeHandle const ret = m_font_manager->createTtf (_buffer, _size);
  if (! isValid (ret))
    return BGFX_INVALID_HANDLE;

  m_ttfs[std::string (_name)] = ret;

  return ret;
}

TrueTypeHandle TextSystem::LoadTTF (std::filesystem::path const &_path, std::string_view _name)
{
  std::error_code ec;
  std::string pth = std::filesystem::canonical(_path, ec).string ();
  if (ec)
    return BGFX_INVALID_HANDLE;

  MappedFile file (pth);
  if (! file.ptr)
    return BGFX_INVALID_HANDLE;

  TrueTypeHandle const ret = LoadTTF ((u8 const *)file.ptr, u32(file.size), _name);
  if (! isValid (ret))
    return BGFX_INVALID_HANDLE;

  m_ttfs_path[pth] = ret;

  return ret;
}

TrueTypeHandle TextSystem::FindTTF (std::string_view _name) const
{
  if (auto it = m_ttfs.find (std::string (_name)); it != m_ttfs.end ())
    return it->second;

  return BGFX_INVALID_HANDLE;
}

TrueTypeHandle TextSystem::FindTTF (std::filesystem::path const &_path) const
{
  std::error_code ec;
  std::string pth = std::filesystem::canonical(_path, ec).string ();
  if (ec)
    return BGFX_INVALID_HANDLE;

  if (auto it = m_ttfs_path.find (_path); it != m_ttfs_path.end ())
    return it->second;

  return BGFX_INVALID_HANDLE;
}

FontHandle TextSystem::CreateFontByPixelSize (TrueTypeHandle _handle, u32 _face_index,
                                              u32 _pixel_size, std::string_view _name)
{
  return CreateFontByPixelSize(_handle, _face_index, _pixel_size, FONT_TYPE_ALPHA, _name);
}

FontHandle TextSystem::CreateFontByPixelSize (TrueTypeHandle _handle, u32 _face_index,
                                              u32 _pixel_size, u32 _font_type,
                                              std::string_view _name)
{
  if (! isValid (_handle))
    return BGFX_INVALID_HANDLE;

  FontHandle const ret = m_font_manager->createFontByPixelSize (_handle, _face_index,
                                                                _pixel_size, _font_type);
  if (! isValid (ret))
    return BGFX_INVALID_HANDLE;

  m_fonts[std::string (_name)] = ret;

  if (! m_font_manager->preloadGlyph (ret, m_preload_set.c_str ()))
    {
      fprintf (stderr, "couldn't load some glpyhs for \"%s\"\n",
               _name.data () ? _name.data () : "");
    }

  return ret;
}

FontHandle TextSystem::FindFont (std::string_view _name) const
{
  if (auto it = m_fonts.find (std::string (_name)); it != m_fonts.end ())
    return it->second;

  return BGFX_INVALID_HANDLE;
}

bgfx::UniformHandle TextSystem::GetAdjColorUniform () const
{
  return m_uni_adj_color;
}

bgfx::UniformHandle TextSystem::GetDimensionsUniform () const
{
  return m_uni_dimensions;
}

bgfx::UniformHandle TextSystem::GetOverUniform () const
{
  return m_uni_over;
}

bgfx::UniformHandle TextSystem::GetUpUniform () const
{
  return m_uni_up;
}


}
