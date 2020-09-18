#include <Matte.hpp>
#include <toml.hpp>

#include <regex>

namespace charm
{

namespace fs = std::filesystem;

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
              zero-filled
  duration - duration of clip covered by matte files,  six digits before the decimal, 3 digits after,
             zero-filled
  descriptor - one-word descriptor of matted element

  structure of clip directory:
  name_descriptor_index
  name - name of film this clip is from, should be same as moviename in toml and directory of clip directories
  descriptor - same descriptor as above
  index - three-digit, zero-filled index of matte in this clip
*/

std::vector<FilmConfiguration>
ReadFilmConfiguration (std::filesystem::path const &_path)
{
  std::error_code ec;

  if (! fs::exists (_path, ec))
    return {};

  fs::path abs_path = fs::absolute(_path.parent_path(), ec);
  if (ec)
    return {};

  std::vector<FilmConfiguration> data;
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
            root = abs_path / p;
          else
            root = p;
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
            roto_root = root / p;
          else
            roto_root = p;
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

          FilmConfiguration fc;
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
  for (FilmConfiguration &fc : data)
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
          ClipDirectory &cd = fc.clips.back ();

          cd.directory = de.path ();
          cd.name = captures[3].str ();
          cd.start_time = std::stod (captures[1].str ());
          cd.duration = std::stod (captures[2].str ());
          cd.frame_count = std::distance (fs::directory_iterator {cd.directory},
                                          fs::directory_iterator {});
        }

      auto less_start_time = [] (ClipDirectory const &l, ClipDirectory const &r)
        {
          return l.start_time < r.start_time ||
            (l.start_time == r.start_time && l.duration < l.duration);
        };
      std::sort (fc.clips.begin (), fc.clips.end (), less_start_time);
    }

  return data;
}

}
