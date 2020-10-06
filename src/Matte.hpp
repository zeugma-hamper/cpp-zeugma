#ifndef MATTE_Y_MATT
#define MATTE_Y_MATT

//#include <bgfx_utils.hpp>
#include <base_types.hpp>

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

}

#endif //MATTE_Y_MATT
