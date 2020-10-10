#ifndef MATTE_Y_MATT
#define MATTE_Y_MATT

//#include <bgfx_utils.hpp>
#include <base_types.hpp>

#include <toml11/toml.hpp>

#include <filesystem>
#include <vector>

namespace charm
{
/*
  structure of film config file:

  top-level - table containing all film details
  root - string containing root path of film files. if not absolute, it is assumed the path is relative to the directory the file is in.
  roto_root - string containing root path to rotoscope images. if not absolute, it is assumed path is relative to root path (see above).

  [movieabbreviation]
  name - display name of film
  path - path to movie file. if not absolute, assumed to be relative to root
  clip_path - path to directory containing directories of matte files. if not absolute, assumed to be relative to roto_root.

  structure of directory of clip directories:
  timestamp_duration_0000000000_descriptor
  timestamp - from beginning of film in second, six digits before the decimal, 3 digits after,
              zero-filled.
  duration - duration of clip covered by matte files,  six digits before the decimal, 3 digits after,
             zero-filled
  descriptor - one-word descriptor of matted element

  structure of clip directory:
  name_descriptor_index
  name - name of film this clip is from, should be same as moviename in toml and directory of clip directories
  descriptor - same descriptor as above
  index - three-digit, zero-filled index of matte in this clip
*/

struct ClipInfo
{
  std::filesystem::path directory;
  std::string name;

  f64 start_time = -1.0;
  f64 duration = -1.0f;
  u32 frame_count = 0u;
};

struct FilmInfo
{
  std::filesystem::path film_path;
  std::filesystem::path clip_path;
  std::string name;
  std::string abbreviation;
  std::vector<ClipInfo> clips;
};

std::vector<FilmInfo>
ReadFilmInfo (std::filesystem::path const &_path);

std::string
MattePathPattern (FilmInfo const _film, ClipInfo const &_clip);

struct MatteGeometry
{
  u32 index = u32(-1);
  u32 dimensions[2] = {0, 0};
  u32 min[2] = {u32(-1), u32 (-1)};
  u32 max[2] = {0, 0};

  static const std::string s_index;
  static const std::string s_dimensions;
  static const std::string s_min;
  static const std::string s_max;

  bool IsValid () const;

  v2f32 GetCenter () const;

  void Print (const char *_prefix = "") const;

  bool operator== (MatteGeometry const &_mg) const;

  toml::table into_toml () const;
  void from_toml (toml::value const &_v);
};

struct MatteDirGeometry
{
  std::string clip_path;
  MatteGeometry dir_geometry;
  std::vector<MatteGeometry> frame_geometry;

  static const std::string s_path;
  static const std::string s_dir_geometry;
  static const std::string s_frame_geometry;

  v2f32 GetCenter () const;

  v2f32 GetCenter (szt _frame) const;

  MatteGeometry const &GetNthFrame (szt _frame) const;

  szt GetFrameCount () const;

  void Sort ();

  bool operator== (MatteDirGeometry const &_mdg) const;

  toml::table into_toml () const;

  void from_toml (toml::value const &_v);

  void Print (bool _per_frame_geom = false) const;
};

struct FilmGeometry
{
  std::string name;
  std::string abbreviation;
  std::vector<MatteDirGeometry> directory_geometries;

  static const std::string s_name;
  static const std::string s_abbreviation;
  static const std::string s_directory_geometries;

  void Sort ();

  bool operator== (FilmGeometry const &_fg) const;

  toml::table into_toml () const;

  void from_toml (toml::value const & _v);
};

std::vector<FilmGeometry>
ReadFileGeometry (std::filesystem::path const &_path);


}

#endif //MATTE_Y_MATT
