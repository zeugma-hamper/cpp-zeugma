#include <TriBand.hpp>

#include <Collage.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <VideoRenderable.hpp>

#include <SumZoft.h>
#include <SinuZoft.h>

#include <random>

namespace charm
{

ElementsBand::ElementsBand (f64 _band_width, f64 _band_height,
                            std::vector<FilmInfo> const &_films)
{
  // middle band - 10 escaped, currently non-mobile, elements
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<>  film_distrib (0, _films.size ()-1);
  std::uniform_real_distribution<> width_distrib (-0.5 * _band_width, 0.5 * _band_width);
  std::uniform_real_distribution<> height_distrib (-0.5 * _band_height, 0.5 * _band_height);
  std::uniform_real_distribution<> scale_distrib (0.3 * _band_height, 0.7 * _band_height);

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
}



TriBand::TriBand (f64 _width, f64 _height, std::vector<FilmInfo> const &_films)
  : Node ()
{
  f64 const band_height = _height / 3.0;
  f64 const band_width = _width;

  f64 const size_factor = 0.3;
  f64 const placement_factor = 1.0 / 3.0;

  Vect const xxx = Vect (1.0, 0.0, 0.0);
  Vect const yyy = Vect (0.0, 1.0, 0.0);

  Vect const collage_center = band_height * yyy;

  std::array<Vect, 3> const positions
    = {collage_center - 1.0 * placement_factor * xxx * band_width,
       collage_center,
       collage_center + 1.0 * placement_factor * xxx * band_width};

  srand48 (435262534);
  //top band - "4" collages
  for (Vect const &v : positions)
    {
      Collage *collage = new Collage (5, _films,
                                      size_factor * band_width,
                                      size_factor * band_height);
      SinuVect perky (0.25 * size_factor * band_height * yyy,
                      1.0 / (2.0 + drand48 ()), ZoftVect(),
                      2.0 * M_PI * drand48 ());
      SinuVect loping (0.35 * size_factor * band_height * xxx,  // yes, not _wid
                       1.0 / (6.0 + drand48 ()), ZoftVect(),
                       2.0 * M_PI * drand48 ());
      SumVect robert (perky, loping);
      collage -> Translate (robert);
      collage->Translate(v);
      AppendChild(collage);
    }

  // middle band - 10 escaped, currently non-mobile, elements
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
