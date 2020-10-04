
#include "conjure-from-toml.h"

#include <map>


namespace charm  {


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
  catch (toml::type_error)
    { fprintf (stderr, "gobbledygook into VectFromTOMLThingy: ");
      return false;
    }
  into_v . Set (x, y, z);
  return true;
}


static std::map <std::string, toml::value> toml_by_filename;


static const toml::value &LoadOrRecallTOMLByFilename (const std::string &fname)
{ auto it = toml_by_filename . find (fname);
  if (it  ==  toml_by_filename . end ())
    { const toml::value tom = toml::parse (fname);
      toml_by_filename[fname] = tom;
      it = toml_by_filename . find (fname);
    }
  return it -> second;
}


i32 NumMaesesFromTOML (const std::string &toml_fpath)
{ try
    { const toml::value &tom = LoadOrRecallTOMLByFilename (toml_fpath);
      const auto all_maeses = toml::find (tom, "maeses");
      return toml::get <toml::array> (all_maeses) . size ();
    }
  catch (...)
    { }
  return -1;
}


PlatonicMaes *MaesFromTOML (const std::string &toml_fpath, i32 index)
{ toml::value tom;

  try
    { tom = toml::parse (toml_fpath); }
  catch (std::runtime_error &e)
    { fprintf (stderr, "MaesFromTOML out in cowardice at th' first step.\n");
      return NULL;
    }
  catch (toml::syntax_error &e)
    { fprintf (stderr, "MaesFromTOML: big ol' syntax error in confix file.\n");
      return NULL;
    }

  const auto all_maeses = toml::find (tom, "maeses");
  toml::value emm;
  try
    { emm = all_maeses . at (index); }
  catch (std::out_of_range &e)
    { fprintf (stderr, "MaesFromTOML: no goldurn maes at index <%d>.\n", index);
      return NULL;
    }

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

  return maes;
}


}  // namespace charm singing duets with mama cass
