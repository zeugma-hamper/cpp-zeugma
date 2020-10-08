#include <base_types.hpp>
#include <Matte.hpp>

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>

#include <toml11/toml.hpp>
#include <iostream>
#include <fstream>

#include <filesystem>
#include <functional>
#include <vector>

#include <stdio.h>

using namespace charm;
namespace fs = std::filesystem;

struct MatteGeometry
{
  u32 index = 0;
  u32 width = 0;
  u32 height = 0;
  u32 min_x = u32(-1);
  u32 max_x = 0;
  u32 min_y = u32(-1);
  u32 max_y = 0;

  bool IsValid () const
  {
    return min_x < max_x && min_y < max_y;
  }

  v2f32 GetCenter () const
  {
    return {(min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f};
  }

  void Print (const char *_prefix)
  {
    printf ("%s[%u, %u]: %u - %u  x  %u - %u\n",
            _prefix, width, height, min_x, max_x, min_y, max_y);
  }

  toml::table into_toml () const
  {
    return toml::table {{"index", index},
                        {"dimensions", {width, height}},
                        {"min", {min_x, min_y}},
                        {"max", {max_x, max_y}}};

  }
};

using ClipRef = std::reference_wrapper<ClipInfo const>;
struct MatteDirGeometry
{
  FilmInfo const *film = nullptr;
  ClipInfo const *clip = nullptr;
  MatteGeometry dir_geometry;
  std::vector<MatteGeometry> frame_geometry;

  v2f32 GetCenter () const
  {
    return dir_geometry.GetCenter ();
  }

  v2f32 GetCenter (szt _frame) const
  {
    if (_frame >= frame_geometry.size ())
      return {-1.0f, -1.0f};

    return frame_geometry[_frame].GetCenter ();
  }

  MatteGeometry const &GetNthFrame (szt _frame) const
  {
    return frame_geometry[_frame];
  }

  szt GetFrameCount () const
  {
    return frame_geometry.size ();
  }

  toml::table into_toml () const
  {
    toml::table geom {{"dir_geometry", dir_geometry.into_toml()},
                      {"frame_geometry", frame_geometry}};
    return toml::table {{film->abbreviation, toml::table {{clip->directory.filename(), geom}}}};
  }

  void Print (bool _per_frame_geom = false)
  {
    dir_geometry.Print ("");
    if (_per_frame_geom)
      for (auto &mg : frame_geometry)
        mg.Print (" ");
  }
};

static MatteDirGeometry process_directory (FilmInfo const &_film, ClipInfo const &_clip);

static int process_mattes (std::vector<FilmInfo> const &_films)
{
  std::vector<MatteDirGeometry> dir_geometry;
  dir_geometry.reserve (_films.size ());

  for (FilmInfo const &fm : _films)
    for (ClipInfo const &cm : fm.clips)
      dir_geometry.push_back (process_directory (fm, cm));

  std::ofstream outfile;
  outfile.open ("mattes.toml");
  for (auto &mdg : dir_geometry)
    outfile << toml::value (mdg);
  outfile.close ();

  return 0;
}

static void CombineGeom (MatteGeometry &_ref, MatteGeometry const &_new)
{
  _ref.min_x = std::min (_ref.min_x, _new.min_x);
  _ref.max_x = std::max (_ref.max_x, _new.max_x);

  _ref.min_y = std::min (_ref.min_y, _new.min_y);
  _ref.max_y = std::max (_ref.max_y, _new.max_y);

  _ref.width  = _ref.max_x - _ref.min_x;
  _ref.height = _ref.max_y - _ref.min_y;
}

static MatteGeometry ConvertToGeom (u32 index, OIIO::ROI const &_roi)
{
  if (_roi.xbegin >= _roi.xend ||
      _roi.ybegin >= _roi.yend)
    return MatteGeometry {};

  return MatteGeometry {index,
                        u32 (_roi.xend - _roi.xbegin),
                        u32 (_roi.yend - _roi.ybegin),
                        u32 (_roi.xbegin), u32 (_roi.xend),
                        u32 (_roi.ybegin), u32 (_roi.yend)};
}

static MatteDirGeometry process_directory (FilmInfo const &_film, ClipInfo const &_clip)
{
  fprintf (stderr, "processing %s\n", _clip.directory.c_str());
  MatteDirGeometry geometry;
  geometry.film = &_film;
  geometry.clip = &_clip;

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
