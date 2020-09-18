#ifndef MATTE_Y_MATT
#define MATTE_Y_MATT

//#include <bgfx_utils.hpp>
#include <base_types.hpp>

#include <filesystem>
#include <vector>

namespace charm
{

struct ClipDirectory
{
  std::filesystem::path directory;
  std::string name;

  f64 start_time = -1.0;
  f64 duration = -1.0f;
  u32 frame_count = 0u;
};

struct FilmConfiguration
{
  std::filesystem::path film_path;
  std::filesystem::path clip_path;
  std::string name;
  std::string abbreviation;
  std::vector<ClipDirectory> clips;
};

std::vector<FilmConfiguration>
ReadFilmConfiguration (std::filesystem::path const &_path);

}

#endif //MATTE_Y_MATT
