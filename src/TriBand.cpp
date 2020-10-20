#include <TriBand.hpp>

#include <Collage.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <VideoRenderable.hpp>

#include "global-params.h"

#include <Frontier.hpp>

#include <random>

namespace charm
{

ElementsBand::ElementsBand (f64 _band_width, f64 _band_height,
                            std::vector<FilmInfo> const &_films,
                            PlatonicMaes &maes, const Vect &cntr)
{

  f64 min_sca = global_param_ee_scale - global_param_ee_scale_delta;
  f64 max_sca = global_param_ee_scale + global_param_ee_scale_delta;
  // middle band - 10 escaped, currently non-mobile, elements
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<>  film_distrib (0, _films.size ()-1);
  std::uniform_real_distribution<> width_distrib (-0.5 * _band_width,
                                                  0.5 * _band_width);
  std::uniform_real_distribution<> height_distrib (-0.5 * _band_height,
                                                   0.5 * _band_height);
  std::uniform_real_distribution<> scale_distrib (min_sca * _band_height,
                                                  max_sca * _band_height);

  Vect ovr = maes . Over ();
  Vect upp = maes . Up ();

  u32 const escapee_count = global_param_ee_count_per_wall;
  for (u32 i = 0; i < escapee_count; ++i)
    {
      FilmInfo const &fm = _films[film_distrib (gen)];
      assert (fm.clips.size () > 1);
      std::uniform_int_distribution<> clip_distrib (0, fm.clips.size ()-1);
      ClipInfo const &cm = fm.clips[clip_distrib (gen)];

      MattedVideoRenderable *matte_able = new MattedVideoRenderable (fm, cm);
      matte_able -> SetOver (ovr);
      matte_able -> SetUp (upp);
      CineAtom *matte_node = new CineAtom;
      matte_node->AppendRenderable (matte_able);
      matte_node->Scale (Vect (scale_distrib (gen)));
      matte_node->Translate (matte_node->loc);
      matte_node->loc
        = cntr  +  width_distrib (gen) * ovr  +  height_distrib (gen) * upp;

      RectRenderableFrontier *rrf
        = new RectRenderableFrontier (matte_able,
                                      -0.5 * (ovr + upp), 0.5 * (ovr + upp));
      matte_node->SetFrontier(rrf);

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

  //top band - "4" collages
  for (Vect const &v : positions)
    {
      Collage *collage = new Collage (5, _films,
                                      size_factor * band_width,
                                      size_factor * band_height);
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
