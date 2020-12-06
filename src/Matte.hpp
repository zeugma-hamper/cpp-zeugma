#ifndef MATTE_Y_MATT
#define MATTE_Y_MATT

//#include <bgfx_utils.hpp>
#include <base_types.hpp>
#include <class_utils.hpp>

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

struct MatteGeometry
{
  u32 index = u32(-1);
  v2u32 dimensions = {0, 0};
  v2u32 min = {u32(-1), u32 (-1)};
  v2u32 max = {0, 0};

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
  std::string niq_atomname;
  MatteGeometry dir_geometry;
  std::vector<MatteGeometry> frame_geometry;

  static const std::string s_path;
  static const std::string s_niq_atomname;
  static const std::string s_dir_geometry;
  static const std::string s_frame_geometry;

  const std::string &UniqueAtomName ()  const
    { return niq_atomname; }

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

struct ClipInfo
{
  std::filesystem::path directory;
  std::string name;
  MatteDirGeometry geometry;

  f64 start_time = -1.0;
  f64 duration = -1.0f;
  f64 end_time = -1.0;
  u32 frame_count = 0u;

  const std::string &UniqueAtomName ()  const
    { return geometry . UniqueAtomName (); }
};


using ClipList = std::vector <const ClipInfo *>;

struct FilmInfo
{
  i64 GetClipCount () const;
  ClipInfo const &GetNthClip (i64 _nth) const;
  std::vector<ClipInfo> const &GetClips () const;

  ClipList GetClipsAfter (ClipInfo const *_clip, f64 _within = 0.0) const;
  ClipList GetClipsAfter (f64 _ts, f64 _within = 0.0) const;

  ClipList ClipsSurrounding (f64 tstamp)  const;
  ClipList ClipsByStartTime ()  const;
  ClipList ClipsByEndTime ()  const;
  const ClipInfo *FirstClipBeginningAfter (f64 tstamp)  const;
  const ClipInfo *FirstClipEndingBefore (f64 tstamp)  const;

  const ClipInfo *ClipFromUniqueAtomName (const std::string &uq_nm)  const;

  std::filesystem::path film_path;
  std::filesystem::path clip_path;
  std::string name;
  std::string abbreviation;
  v2u32 dimensions;
  f64 duration;
  std::vector<ClipInfo> clips;
};

class FilmCatalog
{
 public:
  FilmCatalog ();

  CHARM_DEFAULT_MOVE(FilmCatalog);
  CHARM_DELETE_COPY(FilmCatalog);

  bool LoadFilmInfo (std::filesystem::path const &_path);
  bool LoadFilmGeometry (std::filesystem::path const &_path);

  i64 GetFilmCount () const;
  FilmInfo const &GetNthFilm (i64 _nth) const;
  std::vector<FilmInfo> const &GetFilms () const;
  FilmInfo const *FindFilmByName (std::string_view _name) const;
  FilmInfo const *FindFilmByAbbreviation (std::string_view _abbrev) const;

 private:
  std::vector<FilmInfo> m_films;
  bool m_loaded_films;
  bool m_loaded_geometry;
};


std::vector<FilmInfo>
ReadFilmInfo (std::filesystem::path const &_path);

std::string
MattePathPattern (FilmInfo const _film, ClipInfo const &_clip);

std::vector<FilmGeometry>
ReadFileGeometry (std::filesystem::path const &_path);

void MergeFilmInfoGeometry (std::vector<FilmInfo> &_info,
                            std::vector<FilmGeometry> &_geom);

}

#endif //MATTE_Y_MATT
