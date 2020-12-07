#include <GraphicsApplication.hpp>

#include <Layer.hpp>
#include <Node.hpp>
#include <PlatonicMaes.h>
#include <Renderable.hpp>
#include <TextRenderable.hpp>
#include <TexturedRenderable.hpp>
#include <ZeColor.h>
#include <ZEYowlEvent.h>

#include <BlockTimer.hpp>

#include <base_types.hpp>
#include <class_utils.hpp>
#include <bgfx_utils.hpp>

#include <algorithm>
#include <filesystem>
#include <string_view>

#include <stdio.h>


using namespace charm;
namespace s2 = boost::signals2;
namespace fs = std::filesystem;

static fs::path ttf_path {"/usr/share/fonts/jetbrains-mono/JetBrainsMonoNL-Regular.ttf"};

i64 handle_key_press (s2::connection , ZEYowlAppearEvent *_event)
{
  return 0;
}

int main (int, char **)
{
  GraphicsApplication text_zone;
  if (! text_zone.StartUp ())
    return -1;

  Layer *layer = text_zone.GetSceneLayer();

  Node *nodal = new Node ();
  PlatonicMaes *maes = text_zone.FindMaesByName("front");
  nodal->Scale (0.5 * maes->Width());
  nodal->Translate (maes->Loc());

  TextSystem *txt_sys = TextSystem::GetSystem();
  TrueTypeHandle tt_handle = txt_sys->LoadTTF(ttf_path, "jetbrains mono");
  assert (isValid (tt_handle));

  FontHandle f_handle = txt_sys->CreateFontByPixelSize(tt_handle, 0, 256, "jbm-256");

  TextRenderable *txt_rend = new TextRenderable (f_handle);
  txt_rend->SetTextColor (ZeColor (1.0, 0.0, 0.0, 1.0));
  txt_rend->SetBackgroundColor (ZeColor (0.0, 1.0, 0.0, 1.0));
  txt_rend->EnableBackground ();
  txt_rend->AppendText ("oooooh! dart gun!");
  txt_rend->SetBias (-0.5, 0.0);
  nodal->AppendRenderable (txt_rend);

  s2::connection key_conn
    = text_zone.GetSprinkler().AppendHandler<ZEYowlAppearEvent>(&handle_key_press);

  nodal->AppendRenderable(txt_rend);
  layer->GetRootNode()->AppendChild(nodal);
  CMVTrefoil *tr = text_zone.FindRenderLeafByName("front");
  tr->layers.push_back(layer);

  text_zone.Run ();

  return 0;
}
