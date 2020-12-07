
#include <Matte.hpp>
#include <toml.hpp>

#include <regex>


namespace charm  {

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
          auto dims {toml::get<std::array<u32, 2>> (value.at("dimensions"))};
          fc.dimensions.x = dims[0];
          fc.dimensions.y = dims[1];

          fc.duration = value.at("duration").as_floating();

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
          cd.end_time    = cd.start_time + cd.duration;
          cd.frame_count = std::distance (fs::directory_iterator {cd.directory},
                                          fs::directory_iterator {});
        }

      auto less_dir = [] (ClipInfo const &l, ClipInfo const &r)
        {
          return l.directory < r.directory;
        };
      std::sort (fc.clips.begin (), fc.clips.end (), less_dir);
    }

  std::sort (data.begin (), data.end (),
             [] (FilmInfo const &l, FilmInfo const &r)
             { return l.abbreviation < r.abbreviation; });

  return data;
}

std::string
MattePathPattern (FilmInfo const _film, ClipInfo const &_clip)
{
  return _clip.directory.string () + "/" + _film.abbreviation + "_" + _clip.name + "_" + "%03d.tif";
}


bool MatteGeometry::IsValid () const
{
  return min.x < max.x && min.y < max.y;
}

v2f32 MatteGeometry::GetCenter () const
{
  return {(min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f};
}

void MatteGeometry::Print (const char *_prefix) const
{
  printf ("%s[%u, %u]: %u - %u  x  %u - %u\n",
          _prefix, dimensions.x, dimensions.y, min.x, max.x, min.y, max.y);
}

bool MatteGeometry::operator== (MatteGeometry const &_mg) const
{
  return index == _mg.index &&
    dimensions.x == _mg.dimensions.x  &&
    dimensions.y == _mg.dimensions.y &&
    min.x == _mg.min.x  &&
    max.x == _mg.max.x  &&
    min.y == _mg.min.y  &&
    max.y == _mg.max.y;
}

toml::table MatteGeometry::into_toml () const
{
  std::array<u32, 7> data{index, dimensions.x, dimensions.y,
    min.x, min.y, max.x, max.y};

  return toml::table {{"geom", data}};

  // return toml::table {{s_index, index},
  //                     {s_dimensions, {dimensions.x, dimensions.y}},
  //                     {s_min, {min.x, min.y}},
  //                     {s_max, {max.x, max.y}}};

}

void MatteGeometry::from_toml (toml::value const &_v)
{
  auto data = toml::find<std::array<u32, 7>> (_v, "geom");
  index = data[0];
  dimensions.x = data[1];
  dimensions.y = data[2];
  min.x = data[3];
  min.y = data[4];
  max.x = data[5];
  max.y = data[6];

  // using coord = std::array<u32, 2>;
  // coord tmp;
  // tmp = toml::find<coord> (_v, s_dimensions);
  // dimensions.x = tmp[0];
  // dimensions.y = tmp[1];

  // tmp = toml::find<coord> (_v, s_min);
  // min.x= tmp[0];
  // min.y = tmp[1];

  // tmp = toml::find<coord> (_v, s_max);
  // max.x = tmp[0];
  // max.y = tmp[1];

  // index = toml::find<u32> (_v, s_index);
}

const std::string MatteGeometry::s_index      = "index";
const std::string MatteGeometry::s_dimensions = "dimensions";
const std::string MatteGeometry::s_min = "min";
const std::string MatteGeometry::s_max = "max";

const std::string MatteDirGeometry::s_path = "path";
const std::string MatteDirGeometry::s_niq_atomname = "niq_atomname";
const std::string MatteDirGeometry::s_dir_geometry = "directory_geometry";
const std::string MatteDirGeometry::s_frame_geometry = "frame_geometry";

const std::string FilmGeometry::s_name = "name";
const std::string FilmGeometry::s_abbreviation = "abbreviation";
const std::string FilmGeometry::s_directory_geometries = "directory_geometries";

static const bool s_load_per_frame_geometry = false;

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

  using matte_val = std::array<u32, 7>;
  std::vector<matte_val> frames;
  frames.reserve (frame_geometry.size ());
  for (MatteGeometry const &mg : frame_geometry)
    {
      frames.push_back ({mg.index, mg.dimensions.x, mg.dimensions.y,
          mg.min.x, mg.min.y, mg.max.x, mg.max.y});
    }

  return toml::table {{s_path, clip_path},
                      {s_dir_geometry, dir_geometry},
                      {s_frame_geometry, frames}};
}

void MatteDirGeometry::from_toml (toml::value const &_v)
{
  clip_path = toml::find<std::string> (_v, s_path);
  niq_atomname = toml::find<std::string> (_v, s_niq_atomname);
  dir_geometry = toml::find<MatteGeometry> (_v, s_dir_geometry);
  if (s_load_per_frame_geometry)
    {
      auto const &arr = toml::find<toml::array> (_v, s_frame_geometry);

      std::array<u32, 7> matte_val;
      for (auto &v : arr)
        {
          matte_val = toml::get<std::array<u32, 7>> (v);
          frame_geometry.emplace_back();
          MatteGeometry &mg = frame_geometry.back ();
          mg.index = matte_val[0];
          mg.dimensions.x = matte_val[1];
          mg.dimensions.y = matte_val[2];
          mg.min.x = matte_val[3];
          mg.min.y = matte_val[4];
          mg.max.x = matte_val[5];
          mg.max.y = matte_val[6];
        }
    }
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

i64 FilmInfo::GetClipCount () const
{
  return i64 (clips.size ());
}

ClipInfo const &FilmInfo::GetNthClip (i64 _nth) const
{
  assert (_nth >= 0);
  return clips[_nth];
}

std::vector<ClipInfo> const &FilmInfo::GetClips () const
{
  return clips;
}

std::vector<ClipInfo const *> FilmInfo::GetClipsAfter (ClipInfo const *_clip,
                                                       f64 _within) const
{
  return GetClipsAfter (_clip->start_time + _clip->duration, _within);
}

std::vector<ClipInfo const *> FilmInfo::GetClipsAfter (f64 _ts,
                                                       f64 _within) const
{
  std::vector<ClipInfo const *> ret;

  auto in_range = [] (ClipInfo const &ci, f64 ts)
  { return (ci.start_time <= ts && ts < ci.start_time + ci.duration); };

  auto close_enough = [] (ClipInfo const &base, ClipInfo const &ci, f64 within)
  { return (ci.start_time - base.start_time) <= within + 0.00005;  };

  auto end = clips.end ();
  for (auto cur = clips.begin(); cur != end; ++cur)
    if (in_range(*cur, _ts))
      {
        ClipInfo const &base = *cur;
        ret.push_back (&base);
        ++cur;
        for (; cur != end; ++cur)
          {
            if (close_enough (base, *cur, _within))
              ret.push_back(&*cur);
            else
              return ret;
          }

      }

  return ret;
}


ClipList FilmInfo::ClipsSurrounding (f64 tstamp)  const
{ std::vector <const ClipInfo *> exudate;
  for (auto &ci  :  clips)
    if (ci.start_time <= tstamp  &&  tstamp <= ci.end_time)
      exudate . push_back (&ci);
  return exudate;
}


ClipList FilmInfo::ClipsByStartTime ()  const
{ std::vector <const ClipInfo *> sorty;
  for (auto &ci  :  clips)
    sorty . push_back (&ci);
  auto discrim = [] (const ClipInfo *ca, const ClipInfo *cb)
    { return (ca->start_time < cb->start_time
              &&  ca->end_time < cb->end_time);
    };
  std::sort (sorty . begin (), sorty . end (), discrim);
  return sorty;
}

ClipList FilmInfo::ClipsByEndTime ()  const
{ std::vector <const ClipInfo *> sorty;
  for (auto &ci  :  clips)
    sorty . push_back (&ci);
  auto discrim = [] (const ClipInfo *ca, const ClipInfo *cb)
    { return (ca->end_time > cb->end_time
              &&  ca->start_time > cb->start_time);
    };
  std::sort (sorty . begin (), sorty . end (), discrim);
  return sorty;
}


const ClipInfo *FilmInfo::FirstClipBeginningAfter (f64 tstamp)  const
{ std::vector <const ClipInfo *> clerps = ClipsByStartTime ();
  auto omega = clerps . end ();
  for (auto it = clerps . begin ()  ;  it != omega  ;  ++it)
    if ((*it)->start_time  >  tstamp)
      return *it;
  return NULL;
}

const ClipInfo *FilmInfo::FirstClipEndingBefore (f64 tstamp)  const
{ std::vector <const ClipInfo *> clerps = ClipsByEndTime ();
  auto omega = clerps . end ();
  for (auto it = clerps . begin ()  ;  it != omega  ;  ++it)
    if ((*it)->end_time  <  tstamp)
      return *it;
  return NULL;
}


FilmCatalog::FilmCatalog ()
  : m_loaded_films {false},
    m_loaded_geometry {false}
{
}

bool FilmCatalog::LoadFilmInfo (std::filesystem::path const &_path)
{
  assert (! m_loaded_films);

  m_films = ReadFilmInfo(_path);
  if (m_films.size ())
    {
      m_loaded_films = true;
      return true;
    }

  return false;
}


const ClipInfo
 *FilmInfo::ClipFromUniqueAtomName (const std::string &uq_nm)  const
{ for (const ClipInfo &clin  :  clips)
    if (uq_nm  ==  clin . UniqueAtomName ())
      return &clin;
  return NULL;
}


bool FilmCatalog::LoadFilmGeometry (std::filesystem::path const &_path)
{
  assert (m_loaded_films);
  assert (! m_loaded_geometry);

  std::vector<FilmGeometry> geom = ReadFileGeometry(_path);
  if (geom.size () == 0)
    return false;

  MergeFilmInfoGeometry(m_films, geom);
  m_loaded_geometry = true;
  return true;
}

i64 FilmCatalog::GetFilmCount () const
{
  return i64 (m_films.size ());
}

FilmInfo const &FilmCatalog::GetNthFilm (i64 _nth) const
{
  assert (_nth >= 0);
  return m_films[_nth];
}

std::vector<FilmInfo> const &FilmCatalog::GetFilms () const
{
  return m_films;
}

FilmInfo const *FilmCatalog::FindFilmByName (std::string_view _name) const
{
  for (auto const &fm : m_films)
    if (fm.name == _name)
      return &fm;

  return nullptr;
}

FilmInfo const *FilmCatalog::FindFilmByAbbreviation (std::string_view _abbrev) const
{
  for (auto const &fm : m_films)
    if (fm.abbreviation == _abbrev)
      return &fm;

  return nullptr;
}

std::vector<FilmGeometry>
ReadFileGeometry (std::filesystem::path const &_path)
{
  std::vector<FilmGeometry> parsed_geometry;
  parsed_geometry.reserve (20);

  toml::value const data = toml::parse (_path.string ());
  for (auto const &pr : toml::get<toml::table> (data))
    parsed_geometry.push_back (toml::get<FilmGeometry> (pr.second));

  std::sort (parsed_geometry.begin (), parsed_geometry.end (),
             [] (FilmGeometry const &l, FilmGeometry const &r)
             { return l.abbreviation < r.abbreviation; });

  return parsed_geometry;
}

void MergeFilmInfoGeometry (std::vector<FilmInfo> &_info,
                            std::vector<FilmGeometry> &_geom)
{
  //TODO: optimize. these both should be sorted, so they should match.
  for (FilmInfo &fm : _info)
    {
      auto it = std::find_if (_geom.begin(), _geom.end (),
                              [&fm] (FilmGeometry &g)
                              { return g.abbreviation == fm.abbreviation; });
      if (it == _geom.end ())
        continue;
      FilmGeometry &fgm = *it;

      for (ClipInfo &cm : fm.clips)
        {
          auto cit = std::find_if (fgm.directory_geometries.begin (),
                                   fgm.directory_geometries.end (),
                                   [&cm] (MatteDirGeometry &m)
                                   { return cm.directory.filename() == m.clip_path; });
          if (cit == fgm.directory_geometries.end ())
            continue;

          cm.geometry = std::move (*cit);
        }
    }
}


}  // namespace charm terminated with extreme prejudice
