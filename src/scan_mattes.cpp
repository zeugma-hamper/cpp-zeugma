#include <base_types.hpp>
#include <Matte.hpp>

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>

#include <toml11/toml.hpp>
#include <iostream>
#include <fstream>

#include <array>
#include <filesystem>
#include <functional>
#include <vector>

#include <stdio.h>

using namespace charm;
namespace fs = std::filesystem;

static MatteDirGeometry process_directory (ClipInfo const &_clip);

static int process_mattes (std::vector<FilmInfo> const &_films)
{
  std::vector<FilmGeometry> film_geometry;
  film_geometry.reserve (_films.size ());

  for (FilmInfo const &fm : _films)
    {
      film_geometry.emplace_back();
      FilmGeometry &fg = film_geometry.back ();
      fg.directory_geometries.reserve(10);
      fg.name = fm.name;
      fg.abbreviation = fm.abbreviation;

      for (ClipInfo const &cm : fm.clips)
        fg.directory_geometries.push_back (process_directory (cm));
    }

  std::sort (film_geometry.begin (), film_geometry.end (),
             [] (FilmGeometry const &l, FilmGeometry const &r)
             { return l.abbreviation < r.abbreviation; });

  std::ofstream outfile;
  outfile.open ("mattes.toml");
  for (auto const &fg : film_geometry)
    outfile << toml::value (fg);
  outfile.close ();

#if 0
  std::vector<FilmGeometry> parsed_geometry;
  toml::value const data = toml::parse ("mattes.toml");
  for (auto const &pr : toml::get<toml::table> (data))
    parsed_geometry.push_back (toml::get<FilmGeometry> (pr.second));

  std::sort (parsed_geometry.begin (), parsed_geometry.end (),
             [] (FilmGeometry const &l, FilmGeometry const &r)
             { return l.abbreviation < r.abbreviation; });

  if (film_geometry == parsed_geometry)
    fprintf (stderr, "they are equal\n");
  else
    {
      fprintf (stderr, "they are not equal\n");

      fprintf (stderr, "scanned:\n");
      for (auto const &fg : film_geometry)
        for (auto const &mdg : fg.directory_geometries)
          mdg.Print ();

      fprintf (stderr, "parsed:\n");
      for (auto const &fg : parsed_geometry)
        for (auto const &mdg : fg.directory_geometries)
          mdg.Print ();
    }
#endif

  return 0;
}

static void CombineGeom (MatteGeometry &_ref, MatteGeometry const &_new)
{
  // printf ("ref: (%u, %u) x (%u, %u)\n",
  //         _ref.min.x, _ref.min.y, _ref.max.x, _ref.max.y);
  // printf ("new: (%u, %u) x (%u, %u)\n",
  //         _new.min.x, _new.min.y, _new.max.x, _new.max.y);

  _ref.min.x = std::min (_ref.min.x, _new.min.x);
  _ref.min.y = std::min (_ref.min.y, _new.min.y);

  _ref.max.x = std::max (_ref.max.x, _new.max.x);
  _ref.max.y = std::max (_ref.max.y, _new.max.y);

  _ref.dimensions.x = _ref.max.x - _ref.min.x;
  _ref.dimensions.y = _ref.max.y - _ref.min.y;

  // printf ("dim: (%u, %u)\n", _ref.dimensions.x, _ref.dimensions.y);
}

static MatteGeometry ConvertToGeom (u32 index, OIIO::ROI const &_roi)
{
  if (_roi.xbegin >= _roi.xend ||
      _roi.ybegin >= _roi.yend)
    return MatteGeometry {};

  return MatteGeometry {index,
                        {u32 (_roi.xend - _roi.xbegin),
                         u32 (_roi.yend - _roi.ybegin)},
                        {u32 (_roi.xbegin), u32 (_roi.ybegin)},
                        {u32 (_roi.xend), u32 (_roi.yend)}};
}

static MatteDirGeometry process_directory (ClipInfo const &_clip)
{
  fprintf (stderr, "processing %s\n", _clip.directory.c_str());
  MatteDirGeometry geometry;
  geometry.clip_path = _clip.directory.filename();

  OIIO::ImageBuf image;
  OIIO::ImageBuf inverted_image;

  auto end = fs::directory_iterator{};
  u32 index = 0;
  for (auto cur = fs::directory_iterator{_clip.directory}; cur != end; ++cur)
    {
      image.reset (cur->path().string());
      OIIO::ImageBufAlgo::invert (inverted_image, image);
      OIIO::ROI const roi = OIIO::ImageBufAlgo::nonzero_region (inverted_image);
      MatteGeometry const mg = ConvertToGeom(index++, roi);
      CombineGeom(geometry.dir_geometry, mg);
      geometry.frame_geometry.push_back(mg);
    }

  geometry.dir_geometry.Print ("");
  geometry.Print ();
  return geometry;
}


int main (int ac, char **av)
{
  if (ac < 2)
    {
      fprintf (stderr, "usage: scan_mattes <film_config_file>\n");
      return 1;
    }

  std::error_code ec;
  fs::path config_path = av[1];

  if (! fs::exists (config_path, ec))
    {
      fprintf (stderr, "config file %s not found\n", config_path.c_str());
      return -1;
    }

  std::vector<FilmInfo> films = ReadFilmInfo(config_path);
  if (films.empty())
    {
      fprintf (stderr, "nothing found in config file\n");
      return -1;
    }

  return process_mattes (films);
}
