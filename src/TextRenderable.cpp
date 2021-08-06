
#include <TextRenderable.hpp>

#include <Node.hpp>

#include <vector_interop.hpp>


namespace zeugma  {


TextRenderable::TextRenderable (FontHandle _font)
  : TextRenderable (nullptr, _font, {})
{
}

TextRenderable::TextRenderable (FontHandle _font, std::string_view _view)
  : TextRenderable (nullptr, _font, _view)
{ }

TextRenderable::TextRenderable (Node *_node, FontHandle _font, std::string_view _view)
  : Renderable {_node},
    m_font {_font},
    m_style_flags {0u},
    m_bias_x {0.0f},
    m_bias_y {0.0f}
{
  m_manager = TextSystem::GetTextBufferManager();
  assert (m_manager);

  m_text_buffer = m_manager->createTextBuffer(FONT_TYPE_ALPHA,
                                              BufferType::Dynamic);

  if (_view.data ())
    m_manager->appendText(m_text_buffer, m_font, _view.data ());
}

TextRenderable::~TextRenderable ()
{
  if (m_manager && isValid (m_text_buffer))
    m_manager->destroyTextBuffer(m_text_buffer);

  m_font = BGFX_INVALID_HANDLE;
  m_text_buffer = BGFX_INVALID_HANDLE;
  m_manager = nullptr;
}

void TextRenderable::Draw (u16 _view_id)
{
  TextRectangle const rect = m_manager->getRectangle(m_text_buffer);
  glm::vec4 const dims {rect.width, rect.height, m_bias_x, m_bias_y};

  glm::vec4 const over {as_glm (Over ()), 0.0f};
  glm::vec4 const up {as_glm (Up ()), 0.0f};

  glm::vec4 const color {as_glm (AdjColor())};

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  bgfx::setUniform (TextSystem::GetSystem ()->GetDimensionsUniform (), glm::value_ptr (dims));
  bgfx::setUniform (TextSystem::GetSystem ()->GetOverUniform (), glm::value_ptr (over));
  bgfx::setUniform (TextSystem::GetSystem ()->GetUpUniform (), glm::value_ptr (up));
  bgfx::setUniform (TextSystem::GetSystem ()->GetAdjColorUniform (), glm::value_ptr (color));

  m_manager->submitTextBuffer (m_text_buffer, _view_id , GetSortKey());
}

void TextRenderable::SetBias (f32 _bias_x, f32 _bias_y)
{
  m_bias_x = _bias_x;
  m_bias_y = _bias_y;
}

f32 TextRenderable::GetBiasX () const
{
  return m_bias_x;
}

f32 TextRenderable::GetBiasY () const
{
  return m_bias_y;
}

void TextRenderable::AppendText (std::string_view _text)
{
  AppendText (m_font, _text);
}

void TextRenderable::AppendText (FontHandle _font, std::string_view _text)
{
  m_manager->appendText (m_text_buffer, _font, _text.data ());
}

void TextRenderable::ClearText ()
{
  m_manager->clearTextBuffer(m_text_buffer);
}

static u32 convert_ZeColor (ZeColor const &_color)
{
  auto convert_channel = [] (f32 _chan) -> u8
  {
    return u8 (std::round(_chan * 255.0));
  };

  return  (convert_channel(_color.R()) << 24 |
           convert_channel(_color.G()) << 16 |
           convert_channel(_color.B()) << 8 |
           convert_channel(_color.A()));
}

void TextRenderable::SetDefaultStyle ()
{
  SetStyle (STYLE_NORMAL);
}

static u32 add_flag (u32 _flags, u32 _to_add)
{
  return _flags | _to_add;
}

static u32 remove_flag (u32 _flags, u32 _to_remove)
{
  return (_flags & ~_to_remove);
}

void TextRenderable::SetStyle (u32 _flags)
{
  m_style_flags = _flags;
  m_manager->setStyle(m_text_buffer, m_style_flags);
}

u32 TextRenderable::GetStyle () const
{
  return m_style_flags;
}


void TextRenderable::EnableOverline ()
{
  SetStyle (add_flag (m_style_flags, STYLE_OVERLINE));
}

void TextRenderable::DisableOverline ()
{
  SetStyle (remove_flag (m_style_flags, STYLE_OVERLINE));
}


void TextRenderable::EnableUnderline ()
{
  SetStyle (add_flag (m_style_flags, STYLE_UNDERLINE));
}

void TextRenderable::DisableUnderline ()
{
  SetStyle (remove_flag (m_style_flags, STYLE_UNDERLINE));
}


void TextRenderable::EnableStrikeThrough ()
{
  SetStyle (add_flag (m_style_flags, STYLE_STRIKE_THROUGH));
}

void TextRenderable::DisableStrikeThrough ()
{
  SetStyle (remove_flag (m_style_flags, STYLE_STRIKE_THROUGH));
}


void TextRenderable::EnableBackground ()
{
  SetStyle (add_flag (m_style_flags, STYLE_BACKGROUND));
}

void TextRenderable::DisableBackground ()
{
  SetStyle (remove_flag (m_style_flags, STYLE_BACKGROUND));
}


void TextRenderable::SetFont (FontHandle _font)
{
  if (isValid(_font))
    m_font = _font;
}

void TextRenderable::SetTextColor (ZeColor const &_color)
{
  m_manager->setTextColor (m_text_buffer, convert_ZeColor(_color));
}

void TextRenderable::SetBackgroundColor (ZeColor const &_color)
{
  m_manager->setBackgroundColor (m_text_buffer, convert_ZeColor(_color));
}

void TextRenderable::SetOverlineColor (ZeColor const &_color)
{
  m_manager->setOverlineColor (m_text_buffer, convert_ZeColor(_color));
}

void TextRenderable::SetUnderlineColor (ZeColor const &_color)
{
  m_manager->setUnderlineColor (m_text_buffer, convert_ZeColor(_color));
}

void TextRenderable::SetStrikeThroughColor (ZeColor const &_color)
{
  m_manager->setStrikeThroughColor (m_text_buffer, convert_ZeColor(_color));
}


}
