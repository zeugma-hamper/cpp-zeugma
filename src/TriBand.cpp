#include <TriBand.hpp>

#include <Collage.hpp>
#include <Matte.hpp>
#include <VideoRenderable.hpp>

namespace charm
{

TriBand::TriBand (f64 _width, f64 _height, std::vector<FilmInfo> const &_films)
  : Node ()
{
  f64 const band_height = _height / 3.0;
  f64 const band_width = _width;

  f64 const size_factor = 0.225;
  f64 const placement_factor = 0.125;

  Vect const xxx = Vect (1.0, 0.0, 0.0);
  Vect const yyy = Vect (0.0, 1.0, 0.0);

  //top band
  Vect const collage_center = band_height * yyy;
  Collage *collage = new Collage (5, _films,
                                  size_factor * band_width,
                                  size_factor * band_height);
  collage->Translate(collage_center
                     - 3.0 * placement_factor * xxx * band_width);
  AppendChild(collage);

  Collage *collage2 = new Collage (5, _films,
                                   size_factor * band_width,
                                   size_factor * band_height);
  collage2->Translate(collage_center
                      - placement_factor * xxx * band_width);
  AppendChild(collage2);

  Collage *collage3 = new Collage (5, _films,
                                  size_factor * band_width,
                                  size_factor * band_height);
  collage3->Translate(collage_center
                      + placement_factor * xxx * band_width);
  AppendChild(collage3);

  Collage *collage4 = new Collage (5, _films,
                                   size_factor * band_width,
                                   size_factor * band_height);
  collage4->Translate(collage_center
                      + 3.0 *  placement_factor * xxx * band_width);
  AppendChild(collage4);

  // bottom band
  Vect const video_center = -0.8 * band_height * yyy;
  VideoRenderable *vr = new VideoRenderable (_films[4]);
  Node *video = new Node;
  video->AppendRenderable (vr);
  video->Scale (Vect (0.6 * band_width));
  video->Translate (video_center);
  AppendChild(video);
}

}
