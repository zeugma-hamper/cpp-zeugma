#include <TriBand.hpp>

#include <Collage.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <VideoRenderable.hpp>

#include <random>

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

  // middle band
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<>  film_distrib (0, _films.size ()-1);
  std::uniform_real_distribution<> width_distrib (-0.5 * band_width, 0.5 * band_width);
  std::uniform_real_distribution<> height_distrib (-0.5 * band_height, 0.5 * band_height);
  std::uniform_real_distribution<> scale_distrib (0.3 * band_height, 0.7 * band_height);

  u32 const escapee_count = 10;
  for (u32 i = 0; i < escapee_count; ++i)
    {
      FilmInfo const &fm = _films[film_distrib (gen)];
      assert (fm.clips.size () > 1);
      std::uniform_int_distribution<> clip_distrib (0, fm.clips.size ()-1);
      ClipInfo const &cm = fm.clips[clip_distrib (gen)];

      MattedVideoRenderable *matte_able = new MattedVideoRenderable (fm, cm);
      Node *matte_node = new Node;
      matte_node->AppendRenderable(matte_able);
      matte_node->Scale(Vect (scale_distrib (gen)));
      matte_node->Translate(Vect (width_distrib (gen), height_distrib (gen), 0.0));
      AppendChild (matte_node);
    }

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
