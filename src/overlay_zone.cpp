#include <GraphicsApplication.hpp>

#include <Layer.hpp>
#include <Node.hpp>
#include <PlatonicMaes.h>
#include <Renderable.hpp>
#include <OverlayInstancing.hpp>
#include <OverlayTest.hpp>
#include <ZeColor.h>

#include <BlockTimer.hpp>

#include <base_types.hpp>
#include <class_utils.hpp>
#include <bgfx_utils.hpp>

#include <algorithm>
#include <filesystem>
#include <string_view>

#include <stdio.h>


using namespace charm;
int main (int, char **)
{
  GraphicsApplication text_zone;
  if (! text_zone.StartUp ())
    return -1;

  Layer *layer = text_zone.GetSceneLayer();

  Node *nodal = new Node ();
  PlatonicMaes *maes = text_zone.FindMaesByName("front");

#if 0
  nodal->Scale (maes->Width(), maes->Height(), 1.0);
  nodal->Translate (maes->Loc());
  OverlayTest *renderable = new OverlayTest;
  std::vector<v4f32> points{{0.1f, 0.1f, 0.2f, 0.2f},
                            {0.8f, 0.1f, 0.9f, 0.5f},
                            {0.9f, -0.05f, 1.1f, 0.05f},
                            {0.9f, 0.9f,   1.1f, 1.1f}};
  renderable->SetPoints(points);
  renderable->SetRadius(100.0);
  renderable->SetDimensions (maes->Width(), maes->Height());
#else
  //OverlayInstancing takes a array of points and renders AdjColor'ed circles of
  //configured radius around them.
  nodal->Translate (maes->Loc());
  OverlayInstancing *renderable = new OverlayInstancing;
  Vect v (0.0);
  renderable->SetPoints(&v, 1);
  renderable->SetRadius(0.1 * maes->Height ());
#endif

  nodal->AppendRenderable(renderable);
  layer->GetRootNode()->AppendChild(nodal);
  CMVTrefoil *tr = text_zone.FindRenderLeafByName("front");
  tr->layers.push_back(layer);

  text_zone.Run ();

  return 0;
}
