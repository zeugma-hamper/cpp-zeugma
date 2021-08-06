
#ifndef GLYPH_GLYPH_GLYPH
#define GLYPH_GLYPH_GLYPH

#include <Renderable.hpp>

#include <TextSystem.hpp>

#include <string_view>


namespace zeugma  {


class Node;

//TextRenderable adapts the bgfx text rendering example to zeugma's
//codebase. The most important thing to note is the API below is kinda
//like immediate mode. For example, setting the text color to white,
//appending text, setting the text color to red, then appending more
//text, will get you white text followed by red text.  Secondly,
//SetStyle is used to enable or disable background, underline, etc.

class TextRenderable : public Renderable
{
 public:

  TextRenderable (FontHandle _font);
  TextRenderable (FontHandle _font, std::string_view _text);
  TextRenderable (Node *_node, FontHandle _font, std::string_view _text);

  ~TextRenderable () override;

  ZEUGMA_DELETE_MOVE_COPY(TextRenderable);

  void Draw (u16 _view_id) override;

  // our usual [-0.5, 0.5] grid
  void SetBias (f32 _bias_x, f32 _bias_y);
  f32 GetBiasX () const;
  f32 GetBiasY () const;

	void AppendText (std::string_view _text);
	void AppendText (FontHandle _fontHandle, std::string_view _text);
  void ClearText ();

  //bitfield
  //enum TextStyleFlags
  // {
  // 	STYLE_NORMAL = 0,
  // 	STYLE_OVERLINE = 1,
  // 	STYLE_UNDERLINE = 1 << 1,
  // 	STYLE_STRIKE_THROUGH = 1 << 2,
  // 	STYLE_BACKGROUND = 1 << 3,
  // };
  void SetDefaultStyle ();
  void SetStyle (u32 _flags);
  u32 GetStyle () const;

  void EnableOverline ();
  void DisableOverline ();

  void EnableUnderline ();
  void DisableUnderline ();

  void EnableStrikeThrough ();
  void DisableStrikeThrough ();

  void EnableBackground ();
  void DisableBackground ();

  void SetFont (FontHandle _font);

  void SetTextColor (ZeColor const &_color);

  void SetBackgroundColor (ZeColor const &_color);

	void SetOverlineColor (ZeColor const &_color);

	void SetUnderlineColor (ZeColor const &_color);

	void SetStrikeThroughColor (ZeColor const &_color);


 protected:
  TextBufferManager *m_manager;
  TextBufferHandle m_text_buffer;
  FontHandle m_font;
  u32 m_style_flags;
  f32 m_bias_x, m_bias_y;
};


}


#endif  //GLYPH_GLYPH_GLYPH
