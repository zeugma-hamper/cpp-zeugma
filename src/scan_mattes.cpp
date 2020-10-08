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

struct MatteGeometry
{
  u32 index = 0;
  u32 width = 0;
  u32 height = 0;
  u32 min_x = u32(-1);
  u32 max_x = 0;
  u32 min_y = u32(-1);
  u32 max_y = 0;

  static const std::string s_index;
  static const std::string s_dimensions;
  static const std::string s_min;
  static const std::string s_max;

  bool IsValid () const
  {
    return min_x < max_x && min_y < max_y;
  }

  v2f32 GetCenter () const
  {
    return {(min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f};
  }

  void Print (const char *_prefix) const
  {
    printf ("%s[%u, %u]: %u - %u  x  %u - %u\n",
            _prefix, width, height, min_x, max_x, min_y, max_y);
  }

  bool operator== (MatteGeometry const &_mg) const
  {
    return index  == _mg.index &&
      width  == _mg.width  &&
      height == _mg.height &&
      min_x  == _mg.min_x  &&
      max_x  == _mg.max_x  &&
      min_y  == _mg.min_y  &&
      max_y  == _mg.max_y;
  }

  toml::table into_toml () const
  {
    return toml::table {{s_index, index},
                        {s_dimensions, {width, height}},
                        {s_min, {min_x, min_y}},
                        {s_max, {max_x, max_y}}};

  }

  void from_toml (toml::value const &_v)
  {
    using coord = std::array<u32, 2>;
    coord tmp;
    tmp = toml::find<coord> (_v, s_dimensions);
    width = tmp[0];
    height = tmp[1];

    tmp = toml::find<coord> (_v, s_min);
    min_x = tmp[0];
    min_y = tmp[1];

    tmp = toml::find<coord> (_v, s_max);
    max_x = tmp[0];
    max_y = tmp[1];

    index = toml::find<u32> (_v, s_index);
  }
};

const std::string MatteGeometry::s_index      = "index";
const std::string MatteGeometry::s_dimensions = "dimensions";
const std::string MatteGeometry::s_min = "min";
const std::string MatteGeometry::s_max = "max";

struct MatteDirGeometry
{
  std::string clip_path;
  MatteGeometry dir_geometry;
  std::vector<MatteGeometry> frame_geometry;

  static const std::string s_path;
  static const std::string s_dir_geometry;
  static const std::string s_frame_geometry;

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

  void Sort ()
  {
    std::sort (frame_geometry.begin (), frame_geometry.end (),
               [] (MatteGeometry const &l, MatteGeometry const &r)
               { return l.index < r.index; });
  }

  bool operator== (MatteDirGeometry const &_mdg) const
  {
    return clip_path == _mdg.clip_path &&
      dir_geometry == _mdg.dir_geometry &&
      frame_geometry == _mdg.frame_geometry;
  }

  toml::table into_toml () const
  {
    return toml::table {{s_path, clip_path},
                        {s_dir_geometry, dir_geometry},
                        {s_frame_geometry, frame_geometry}};
  }

  void from_toml (toml::value const &_v)
  {
    clip_path = toml::find<std::string> (_v, s_path);
    dir_geometry = toml::find<MatteGeometry> (_v, s_dir_geometry);
    frame_geometry = toml::find<std::vector<MatteGeometry>> (_v, s_frame_geometry);
  }

  void Print (bool _per_frame_geom = false) const
  {
    dir_geometry.Print ("");
    if (_per_frame_geom)
      for (auto &mg : frame_geometry)
        mg.Print (" ");
  }
};

const std::string MatteDirGeometry::s_path = "path";
const std::string MatteDirGeometry::s_dir_geometry = "directory_geometry";
const std::string MatteDirGeometry::s_frame_geometry = "frame_geometry";

struct FilmGeometry
{
  std::string name;
  std::string abbreviation;
  std::vector<MatteDirGeometry> directory_geometries;

  static const std::string s_name;
  static const std::string s_abbreviation;
  static const std::string s_directory_geometries;

  void Sort ()
  {
    std::sort (directory_geometries.begin (), directory_geometries.end (),
               [] (MatteDirGeometry const &l, MatteDirGeometry const &r) { return l.clip_path < r.clip_path; });

    for (auto &mdg : directory_geometries)
      mdg.Sort ();
  }

  bool operator== (FilmGeometry const &_fg) const
  {
    return name == _fg.name &&
      abbreviation == _fg.abbreviation &&
      directory_geometries == _fg.directory_geometries;
  }

  toml::table into_toml () const
  {
    return toml::table {{abbreviation, toml::table {{s_name, name},
                                                    {s_abbreviation, abbreviation},
                                                    {s_directory_geometries, directory_geometries}}}};
  }

  void from_toml (toml::value const & _v)
  {
    name = toml::find<std::string> (_v, s_name);
    abbreviation = toml::find<std::string> (_v, s_abbreviation);
    directory_geometries = toml::find<std::vector<MatteDirGeometry>> (_v, s_directory_geometries);

    Sort ();
  }
};

const std::string FilmGeometry::s_name = "name";
const std::string FilmGeometry::s_abbreviation = "abbreviation";
const std::string FilmGeometry::s_directory_geometries = "directory_geometries";


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
