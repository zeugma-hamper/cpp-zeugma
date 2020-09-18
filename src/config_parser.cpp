#include <Matte.hpp>

#include <stdio.h>

using namespace charm;

static void print_film_info (FilmInfo const &_fc)
{
  printf ("%s (%s):\n", _fc.name.c_str (), _fc.abbreviation.c_str());
  printf (" path: %s\n", _fc.film_path.c_str());
  printf (" clip path: %s\n", _fc.clip_path.c_str());
  printf (" clip count: %zu\n", _fc.clips.size ());
}

static void print_clip_information (ClipInfo const &_cd, const char *_prefix = "")
{
  printf ("%sname: %s\n", _prefix, _cd.name.c_str());
  printf ("%spath: %s\n", _prefix, _cd.directory.c_str());
  printf ("%stime: %0.3f\n", _prefix, _cd.start_time);
  printf ("%sduration: %.3f\n", _prefix, _cd.duration);
  printf ("%scount: %u\n", _prefix, _cd.frame_count);
}

int main (int ac, char **av)
{
  if (ac < 2)
    {
      fprintf (stderr, "usage: config_parser <path>\n");
      return -1;
    }

  fprintf (stderr, "reading %s\n", av[1]);
  std::vector<FilmInfo> configs =
    ReadFilmInfo(av[1]);

  if (configs.size () == 0)
    {
      printf ("file contains no film configuration data\n");
      return 0;
    }

  for (FilmInfo const &fc : configs)
    print_film_info (fc);

  printf ("\n");
  for (ClipInfo &cd : configs[0].clips)
    {
      print_clip_information(cd);
      printf ("\n");
    }

  return 0;
}
