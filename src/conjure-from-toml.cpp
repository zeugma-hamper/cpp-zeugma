
#include "conjure-from-toml.h"

#include <toml.hpp>
#include <map>


namespace zeugma  {


static std::map <std::string, toml::value> toml_by_filename;


const toml::value &LoadOrRecallTOMLByFilename (const std::string &fname)
{ auto it = toml_by_filename . find (fname);
  if (it  ==  toml_by_filename . end ())
    { const toml::value tom = toml::parse (fname);
      toml_by_filename[fname] = tom;
      it = toml_by_filename . find (fname);
    }
  return it -> second;
}


bool VectFromTOMLThingy (const toml::value &thingy, Vect &into_v)
{ f64 x, y, z;
  try
    { auto val = thingy[0];
      x = toml::get <f64> (val);
      val = thingy[1];
      y = toml::get <f64> (val);
      val = thingy[2];
      z = toml::get <f64> (val);
    }
  catch (std::out_of_range &e)
    { fprintf (stderr, "VectFromTOMLThingy: doesn't have 0/1/2 indices.\n");
      return false;
    }
  catch (toml::type_error &)
    { fprintf (stderr, "gobbledygook into VectFromTOMLThingy: ");
      return false;
    }
  into_v . Set (x, y, z);
  return true;
}


bool Matrix44FromTOMLThingy (const toml::value &thingy, Matrix44 &into_m)
{ Matrix44 mat;
  f64 *mp = &mat.a_[0];
  for (i32 q = 0  ;  q < 16  ;  ++q)
    try
      { auto val = thingy[q];
        *mp++ = toml::get <f64> (val);
      }
    catch (std::out_of_range &e)
      { fprintf (stderr, "VectFromTOMLThingy: doesn't have 0/1/2 indices.\n");
        return false;
      }
    catch (toml::type_error &)
      { fprintf (stderr, "gobbledygook into Matrix44FromTOMLThingy: ");
        return false;
      }
  into_m . Load (mat);
  return true;
}


bool PointAndDirecTransformMatsFromTOML (const std::string &toml_fpath,
                                         Matrix44 &pmat, Matrix44 &dmat)
{ try
    { const toml::value &tom = LoadOrRecallTOMLByFilename (toml_fpath);
      Matrix44 pee_matty (INITLESS);
      Matrix44 dee_matty (INITLESS);
      const toml::value emm_p = toml::find (tom, "point_mat");
      if (! Matrix44FromTOMLThingy (emm_p, pee_matty))
        return false;
      const toml::value emm_d = toml::find (tom, "direc_mat");
      if (! Matrix44FromTOMLThingy (emm_d, dee_matty))
        return false;
      pmat . Load (pee_matty);
      dmat . Load (dee_matty);
      return true;
    }
  catch (...)
    { }
  return false;
}


i32 NumMaesesFromTOML (const std::string &toml_fpath)
{ try
    { const toml::value &tom = LoadOrRecallTOMLByFilename (toml_fpath);
      const auto all_maeses = toml::find (tom, "maeses");
      return toml::get <toml::array> (all_maeses) . size ();
    }
  catch (std::runtime_error &_error)
    {
      fprintf (stderr, "error loading %s:\n%s\n",
               toml_fpath.c_str (), _error.what ());
    }
  catch (toml::exception &_error)
    {
      fprintf (stderr, "toml error %s:%s\n%s\n",
               _error.location().file_name().c_str(),
               _error.location().line_str().c_str(),
               _error.what());
    }

  return -1;
}


PlatonicMaes *MaesFromTOML (const std::string &toml_fpath, i32 index)
{ toml::value tom;
  toml::value emm;

  try
    { tom = toml::parse (toml_fpath);
      const auto all_maeses = toml::find (tom, "maeses");
      emm = all_maeses . at (index);

      PlatonicMaes *maes = new PlatonicMaes;
      if (emm . contains ("name"))
        maes -> SetName (toml::get <toml::string> (emm["name"]));

      Vect v;
      if (emm . contains ("location")
          &&  (VectFromTOMLThingy (emm["location"], v)))
        maes -> SetLoc (v);

      if (emm . contains ("over")
          &&  (VectFromTOMLThingy (emm["over"], v)))
        maes -> SetOver (v);

      if (emm . contains ("up")
          &&  (VectFromTOMLThingy (emm["up"], v)))
        maes -> SetUp (v);

      if (emm . contains ("width"))
        maes -> SetWidth (toml::get <f64> (emm["width"]));

      if (emm . contains ("height"))
        maes -> SetHeight (toml::get <f64> (emm["height"]));

      if (emm . contains ("ideal-pixwid"))
        maes -> SetIdealPixelWidth (toml::get <i64> (emm["ideal-pixwid"]));

      if (emm . contains ("ideal-pixhei"))
        maes -> SetIdealPixelHeight (toml::get <i64> (emm["ideal-pixhei"]));

      return maes;
    }
  catch (std::runtime_error &e)
    { fprintf (stderr, "MaesFromTOML out in cowardice at th' first step.\n");
      return NULL;
    }
  catch (toml::syntax_error &e)
    { fprintf (stderr, "MaesFromTOML: big ol' syntax error in confix file.\n");
      return NULL;
    }
  catch (std::out_of_range &e)
    { fprintf (stderr, "MaesFromTOML: no goldurn maes at index <%d>.\n", index);
      return NULL;
    }

  return NULL;
}

static const char *s_window_toml_file_windows = "windows";
static const char *s_window_toml_file_trefoils = "trefoils";


static const char *s_window_toml_name = "name";
static const char *s_window_toml_pos = "position";
static const char *s_window_toml_size = "size";
static const char *s_window_toml_rgba = "color-size";
static const char *s_window_toml_ds = "ds-size";
static const char *s_window_toml_decorated = "decorated";

static const char *s_trefoil_toml_name = "name";
static const char *s_trefoil_toml_maes = "maes";
static const char *s_trefoil_toml_offset = "offset";
static const char *s_trefoil_toml_size = "size";

void WindowTOML::from_toml (toml::value const &_v)
{
  name = toml::find<std::string> (_v, s_window_toml_name);
  pos = toml::find<std::array<i32, 2>> (_v, s_window_toml_pos);
  size = toml::find<std::array<u32, 2>> (_v, s_window_toml_size);
  rgba = toml::find<std::array<u8, 4>> (_v, s_window_toml_rgba);
  ds = toml::find<std::array<u8, 2>> (_v, s_window_toml_ds);
  decorated = toml::find<bool> (_v, s_window_toml_decorated);
}

void TrefoilTOML::from_toml (toml::value const &_v)
{
  name = toml::find<std::string> (_v, s_trefoil_toml_name);
  maes_name = toml::find<std::string> (_v, s_trefoil_toml_maes);
  offset = toml::find<std::array<u32, 2>> (_v, s_trefoil_toml_offset);
  size = toml::find<std::array<u32, 2>> (_v, s_trefoil_toml_size);
}

bool ReadWindowTOMLFile (std::string_view _file, WindowTOML &_window,
                         std::vector<TrefoilTOML> &_views)
{
  try
    {
      auto config = toml::parse (_file);
      std::vector<WindowTOML> wins = toml::find <std::vector <WindowTOML>>
        (config, s_window_toml_file_windows);
      _window = std::move (wins[0]);
      _views = toml::find <std::vector <TrefoilTOML>>
        (config, s_window_toml_file_trefoils);
    }
  catch (std::runtime_error &_error)
    {
      fprintf (stderr, "error loading %s:\n%s\n",
               _file.data(), _error.what ());

      return false;
    }
  catch (toml::exception &_error)
    {
      fprintf (stderr, "toml error %s:%s\n%s\n",
               _error.location().file_name().c_str(),
               _error.location().line_str().c_str(),
               _error.what());
      return false;
    }

  return true;
}

}  // namespace zeugma singing duets with mama cass
