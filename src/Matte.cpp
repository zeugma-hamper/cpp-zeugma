#include <Matte.hpp>
#include <toml.hpp>

#include <regex>

namespace charm
{

namespace fs = std::filesystem;

std::vector<FilmInfo>
ReadFilmInfo (std::filesystem::path const &_path)
{
  std::error_code ec;

  if (! fs::exists (_path, ec))
    return {};

  fs::path abs_path = fs::canonical(_path.parent_path(), ec);
  if (ec)
    return {};

  std::vector<FilmInfo> data;
  fs::path root;
  fs::path roto_root;

  try
    {
      auto config = toml::parse (_path.c_str());

      // get root film directory from top-level
      if (config.contains("root"))
        {
          fs::path p {toml::find<std::string> (config, "root")};
          if (p.is_relative())
            root = fs::canonical (abs_path / p, ec);
          else
            root = fs::canonical (p, ec);
        }
      else
        {
          root = abs_path;
        }

      // get rotoscope mattes root directory from top-level
      if (config.contains("roto_root"))
        {
          fs::path p {toml::find<std::string> (config, "roto_root")};
          if (p.is_relative())
            roto_root = fs::canonical (root / p, ec);
          else
            roto_root = fs::canonical (p, ec);
        }
      else
        {
          root = abs_path;
        }

      fprintf (stderr, "root is %s\n", root.c_str());
      fprintf (stderr, "roto is %s\n", roto_root.c_str());

      // iterate through all key-value pairs in config. all table
      // values are assumed to be config values about a particular
      // film.
      for (const auto &[key, value] : config.as_table())
        {
          if (! value.is_table())
            continue;

          FilmInfo fc;
          fc.abbreviation = key;
          fc.name = value.at("name").as_string ();

          fs::path tmp {toml::get<std::string> (value.at("path"))};
          if (tmp.is_relative ())
            fc.film_path = root / tmp;
          else
            fc.film_path = tmp;

          tmp = toml::get<std::string> (value.at("clip_path"));
          if (tmp.is_relative())
            fc.clip_path = roto_root / tmp;
          else
            fc.clip_path = tmp;

          // don't new info into list until the end because of exceptions
          data.push_back(fc);
        }
    }
  catch (std::runtime_error &_error)
    {
      fprintf (stderr, "error loading %s:\n%s\n",
               _path.c_str(), _error.what ());
    }
  catch (toml::exception &_error)
    {
      fprintf (stderr, "toml error %s:%s\n%s\n",
               _error.location().file_name().c_str(),
               _error.location().line_str().c_str(),
               _error.what());
    }

  // ok, so now we've parsed out the paths of the video files and the
  // path to the clips.

  // next step: from each clip directory, read and parse the directory
  // names to create a list of clips from each film.

  // aside: now i have two problems
  // example dir name: 001251.083_000004.750_0000000000_vomit
  std::regex const dir_name_ex{"(\\d+\\.\\d+)_(\\d+\\.\\d+)_\\d+_(\\w+)"};
  for (FilmInfo &fc : data)
    {
      if (fc.clip_path.empty() ||
          ! fs::is_directory(fc.clip_path, ec))
        continue;

      for (fs::directory_entry const &de : fs::directory_iterator(fc.clip_path))
        {
          if (! de.is_directory(ec))
            continue;

          // filename() gets us everything after the last slash
          // ex: "/tlp/mattes/boaa/violin"  -> "violin"
          // ex: "/tlp/mattes/boaa/violin/" -> ""
          std::string dir_name = de.path().filename ();
          if (dir_name.empty ())
            {
              fprintf (stderr, "empty dir name for %s\n", de.path ().c_str ());
              continue;
            }

          std::smatch captures;
          if (! std::regex_match (dir_name, captures, dir_name_ex) ||
              captures.size () != 4)
            {
              fprintf (stderr, "can't parse name %s\n", dir_name.c_str ());
              continue;
            }

          fc.clips.emplace_back();
          ClipInfo &cd = fc.clips.back ();

          cd.directory = de.path ();
          cd.name = captures[3].str ();
          cd.start_time  = std::stod (captures[1].str ());
          cd.duration    = std::stod (captures[2].str ());
          cd.frame_count = std::distance (fs::directory_iterator {cd.directory},
                                          fs::directory_iterator {});
        }

      auto less_dir = [] (ClipInfo const &l, ClipInfo const &r)
        {
          return l.directory < r.directory;
        };
      std::sort (fc.clips.begin (), fc.clips.end (), less_dir);
    }

  return data;
}

std::string
MattePathPattern (FilmInfo const _film, ClipInfo const &_clip)
{
  return _clip.directory.string () + "/" + _film.abbreviation + "_" + _clip.name + "_" + "%03d.tif";
}


bool MatteGeometry::IsValid () const
{
  return min_x < max_x && min_y < max_y;
}

v2f32 MatteGeometry::GetCenter () const
{
  return {(min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f};
}

void MatteGeometry::Print (const char *_prefix) const
{
  printf ("%s[%u, %u]: %u - %u  x  %u - %u\n",
          _prefix, width, height, min_x, max_x, min_y, max_y);
}

bool MatteGeometry::operator== (MatteGeometry const &_mg) const
{
  return index  == _mg.index &&
    width  == _mg.width  &&
    height == _mg.height &&
    min_x  == _mg.min_x  &&
    max_x  == _mg.max_x  &&
    min_y  == _mg.min_y  &&
    max_y  == _mg.max_y;
}

toml::table MatteGeometry::into_toml () const
{
  return toml::table {{s_index, index},
                      {s_dimensions, {width, height}},
                      {s_min, {min_x, min_y}},
                      {s_max, {max_x, max_y}}};

}

void MatteGeometry::from_toml (toml::value const &_v)
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

const std::string MatteGeometry::s_index      = "index";
const std::string MatteGeometry::s_dimensions = "dimensions";
const std::string MatteGeometry::s_min = "min";
const std::string MatteGeometry::s_max = "max";

const std::string MatteDirGeometry::s_path = "path";
const std::string MatteDirGeometry::s_dir_geometry = "directory_geometry";
const std::string MatteDirGeometry::s_frame_geometry = "frame_geometry";

const std::string FilmGeometry::s_name = "name";
const std::string FilmGeometry::s_abbreviation = "abbreviation";
const std::string FilmGeometry::s_directory_geometries = "directory_geometries";

v2f32 MatteDirGeometry::GetCenter () const
{
  return dir_geometry.GetCenter ();
}

v2f32 MatteDirGeometry::GetCenter (szt _frame) const
{
  if (_frame >= frame_geometry.size ())
    return {-1.0f, -1.0f};

  return frame_geometry[_frame].GetCenter ();
}

MatteGeometry const &MatteDirGeometry::GetNthFrame (szt _frame) const
{
  return frame_geometry[_frame];
}

szt MatteDirGeometry::GetFrameCount () const
{
  return frame_geometry.size ();
}

void MatteDirGeometry::Sort ()
{
  std::sort (frame_geometry.begin (), frame_geometry.end (),
             [] (MatteGeometry const &l, MatteGeometry const &r)
             { return l.index < r.index; });
}

bool MatteDirGeometry::operator== (MatteDirGeometry const &_mdg) const
{
  return clip_path == _mdg.clip_path &&
    dir_geometry == _mdg.dir_geometry &&
    frame_geometry == _mdg.frame_geometry;
}

toml::table MatteDirGeometry::into_toml () const
{
  return toml::table {{s_path, clip_path},
                      {s_dir_geometry, dir_geometry},
                      {s_frame_geometry, frame_geometry}};
}

void MatteDirGeometry::from_toml (toml::value const &_v)
{
  clip_path = toml::find<std::string> (_v, s_path);
  dir_geometry = toml::find<MatteGeometry> (_v, s_dir_geometry);
  frame_geometry = toml::find<std::vector<MatteGeometry>> (_v, s_frame_geometry);
}

void MatteDirGeometry::Print (bool _per_frame_geom) const
{
  dir_geometry.Print ("");
  if (_per_frame_geom)
    for (auto &mg : frame_geometry)
      mg.Print (" ");
}

void FilmGeometry::Sort ()
{
  std::sort (directory_geometries.begin (), directory_geometries.end (),
             [] (MatteDirGeometry const &l, MatteDirGeometry const &r) { return l.clip_path < r.clip_path; });

  for (auto &mdg : directory_geometries)
    mdg.Sort ();
}

bool FilmGeometry::operator== (FilmGeometry const &_fg) const
{
  return name == _fg.name &&
    abbreviation == _fg.abbreviation &&
    directory_geometries == _fg.directory_geometries;
}

toml::table FilmGeometry::into_toml () const
{
  return toml::table {{abbreviation, toml::table {{s_name, name},
                                                  {s_abbreviation, abbreviation},
                                                  {s_directory_geometries, directory_geometries}}}};
}

void FilmGeometry::from_toml (toml::value const & _v)
{
  name = toml::find<std::string> (_v, s_name);
  abbreviation = toml::find<std::string> (_v, s_abbreviation);
  directory_geometries = toml::find<std::vector<MatteDirGeometry>> (_v, s_directory_geometries);

  Sort ();
}

}
