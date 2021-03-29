
#include "tamparams.h"
#include "TampoChief.h"

#include "Orksur.h"

#include "GraumanPalace.h"
#include "OeuvreAfterlife.h"
#include "AtomicFreezone.h"

#include "TexturedRenderable.hpp"

#include "SinuZoft.h"

#include "ZeUUID.h"

#include "vector_utils.hpp"

#include "GraphicsApplication.hpp"

#include <vector>


std::vector <Jigglegon *> Orksur::fallow_jigs;
//std::vector <Jigglegon *> Orksur::active_jigs;


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false),
                                            underlying_maes (&ma),
                                            assembly (new Node),
                                            // soncho (new SonoChoosist (&ma)),
                                            associated_wallmaes (NULL),
                                            associated_cinelib (NULL),
                                            ascension_phase (-1),
                                            ascending_collage (NULL),
                                            valhalla (NULL),
                                            sentient_dist (200.0),
                                            contact_dist (25.0)
{ AppendChild (assembly);
  soncho = new SonoChoosist (&ma, ma . Height (), Vect::yaxis, -Vect::xaxis);
  AppendChild (soncho);
  soncho -> PopulateChoizls (5);
  // soncho->loc . Set (Loc ()  +  0.5 * (soncho->thck + 2.0 * soncho->brd_thc
  //                                      - ma . Height ()) * ma . Up ());
  soncho->loc . Set (Loc ()  +  0.5 * (ma . Width () - soncho->thck
                                       - 2.0 * soncho->brd_thc) * ma . Over ());
  soncho -> Furl ();
  soncho->contact_dist = this->contact_dist;

  // hoping that this absorbs the disk-access burp...
  TextureParticulars ellipse_tepa
    = CreateTexture2D ("../ui-visuals/ellipse-blanc-99px-blur.png",
                       DefaultTextureFlags);

  asc_slipcover = new Alignifer;

  TextureParticulars slip_base_tp
    = CreateTexture2D ("../ui-visuals/slipcover-base.png", DefaultTextureFlags);
  TexturedRenderable *slip_base_re = new TexturedRenderable (slip_base_tp);
  slip_base_re -> AdjColorZoft () . Set (ZeColor (0.7, 0.65, 0.15, 1.0));
  Node *slip_base_no = new Node (slip_base_re);
  asc_slipcover -> AppendChild (slip_base_no);

  TextureParticulars slip_iris_tp
    = CreateTexture2D ("../ui-visuals/slipcover-iris.png", DefaultTextureFlags);
  TexturedRenderable *slip_iris_re = new TexturedRenderable (slip_iris_tp);
  Node *slip_iris_no = new Node (slip_iris_re);
  slip_iris_no -> Rotate (ZoftVect (Vect::zaxis),
                          LoopFloat (0.0, 2.0 * M_PI / 8.5, 8.5));
  asc_slipcover -> AdjColorZoft () . BecomeLike (asc_covr_fader);
  asc_slipcover -> AppendChild (slip_iris_no);

  asc_hvn_incrsn_zeit . SetTimeFlowRate (-1.0);
  asc_hvn_incrsn_zeit . SetTime (-1.0);

  RetrieveValhalla ();
}


/*
Splort *Orksur::NewSplort (f64 rad, i64 num_verts)  const
{ Splort *spl = new Splort;

  for (i64 w = 0  ;  w < 2  ;  ++w)
    for (i64 q = 0  ;  q < num_verts  ;  ++q)
      { f64 theeeta = 2.0 * M_PI / (f64)num_verts * (f64)q  +  w * M_PI;
        Vect radial = cos (theeeta) * ovr  +  sin (theeeta) * upp;
        radial *= 0.5 * (w + 1.0);
        SinuVect arm (0.065 * rad * radial, 5.0 + 0.7 * drand48 (),
                      0.24 * (1.0 + 3.0 * (q%2)) * rad * radial);
        SumVect voitecks (spl->loc, arm);
        (w > 0 ? spl->re1 : spl->re2) -> AppendVertex (voitecks);
      }

  return spl;
}
*/


stringy_list Orksur::CollageAtomsNameList ()
{ stringy_list nlist;
  for (Ticato *tic  :  players)
    if (tic)
      nlist . push_back (tic -> AtomName ());
  return nlist;
}


Alignifer *Orksur::PermaFixCollage ()
{ Alignifer *collage = new Alignifer;
  const Vect &cnt = Loc ();
  Vect o = Over () . Norm ();
  Vect u = Up () . Norm ();

  // need to neutralizes hovering and grasping, & et-al-ing...
  hoverees . clear ();
  graspees . clear ();
  sel_atom = NULL;

  // what else do we need to turn off? jigglegons, presumably...
  // and ah yes: reset the state of soncho...
  soncho -> InitiateAtomicContact (NULL);
  soncho -> Furl ();

  // a luxurious background-bed for the beatified atoms:
  TextureParticulars ellipse_tepa
    = CreateTexture2D ("../ui-visuals/ellipse-blanc-99px-blur.png",
                       DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (ellipse_tepa);
  Node *texno = new Node (texre);
  collage -> AppendChild (texno);
  f64 grayness = Tamparams::Current ()->asc_collage_background_grey_val;
  InterpColor fader (ZeColor (grayness, 0.0), ZeColor (grayness, 1.0),
                     Tamparams::Current ()->asc_collage_background_appear_time);
  texre -> AdjColorZoft () . BecomeLike (fader);
  texno -> Scale (Width ());

  Alignifer *molecule = new Alignifer;
  molecule -> SetName ("participating-atoms");
  molecule -> AdjColorZoft () . BecomeLike (asc_coll_fader);
  asc_coll_fader . SetHard (ZeColor (1.0, 1.0));
  collage -> AppendChild (molecule);

  ascendees . clear ();

  std::vector <Node *> ticlist = assembly -> ChildListCopy ();
  for (Node *no  :  ticlist)
    if (Ticato *tic = dynamic_cast <Ticato *> (no))
      { // collage -> AppendChild (tic);  // implicit excision from assembly
        molecule -> AppendChild (tic);  // implicit excision from assembly
        ascendees . push_back (tic);

        auto it = std::find (players . begin (), players . end (), tic);
        if (it  !=  players . end ())
          players . erase (it);
        else
          assert (&"kirk"  ==  &"gorn");

        if (tic->aura)
          { tic -> RemoveChild (tic->aura);  // should really recycle...
            tic->aura = NULL;
          }

        Vect cured_pos = tic -> CurLoc () - cnt;
        cured_pos = (cured_pos . Dot (o) * Vect::xaxis
                     +  cured_pos . Dot (u) * Vect::yaxis);
        tic -> LocZoft () . Set (cured_pos);
        tic -> AlignToOther (collage);
      }

  assert (assembly -> ChildCount ()  ==  0);
  assert (players . size ()  ==  0);

  return collage;
}


void Orksur::EffectAscension ()
{ Alignifer *colla = PermaFixCollage ();

  ascending_collage = colla;
  EffectNextAscensionPhase ();
}


void Orksur::ConcludeAscension ()
{ if (OeuvreAfterlife *valhalla = RetrieveValhalla ())
    if (Ollag *cntr_coll = valhalla -> CollageFromOrdinal (0))
      { ZoftVect nullpos (Vect (0.0, 0.0, 0.0));
        ascending_collage -> InstallLocGrapplerZoft (nullpos);
        Vect sc = ascending_collage -> CurScale ();
        sc /= cntr_coll -> CurScale (); // /= (1.8 * Width ());
        ZoftVect neutral_sc ((Vect (sc)));  // delightful, o great vexcrap
        ascending_collage -> InstallScaleGrapplerZoft (neutral_sc);

        cntr_coll -> AppendChild (ascending_collage);

        for (Ticato *tic  :  ascendees)
          if (tic)
            tic -> SonoSilence ();
        ascendees . clear ();
      }
  ascension_phase = -1;
}


bool Orksur::AscensionPhaseJustNowDone ()
{ Tamparams *tam = Tamparams::Current ();
  switch (ascension_phase)
    { case ASCPH_TABLE_SLIDE:
        return asc_table_slide . Replete ();

      case ASCPH_FIRST_RISE:
        return asc_first_rise . Replete ();

      case ASCPH_ENBLOATEN:
        return asc_perf_bloat . Replete ();

      case ASCPH_BEFORE_PRESO:
        return (asc_hold_zeit . CurTime ()
                >  tam->asc_before_preso_hold_time);

      case ASCPH_PRESENTATION:
        return (asc_perf_zeit . CurTime ()
                >  tam->asc_presentation_time);

      case ASCPH_AFTER_PRESO:
        return (asc_hold_zeit . CurTime ()
                >  tam->asc_after_preso_hold_time);

      case ASCPH_ENSVELTEN:
        return asc_perf_bloat . Replete ();

      case ASCPH_SECOND_RISE:
        return asc_final_rise . Replete ();

      default:
        break;
    }

  return false;
}


void Orksur::IndulgeAscensionInterstitials ()
{ Tamparams *tam = Tamparams::Current ();
  Tampo *tampy = Tamglobals::Only ()->solo_tamp;

  switch (ascension_phase)
    { case ASCPH_TABLE_SLIDE:
        break;

      case ASCPH_FIRST_RISE:
        if (Tamparams::Current ()->ascension_auto_follow
            &&  ! asc_autofollow_triggered
            &&  asc_phase_stopwatch . CurTimeGlance ()  >  0.70 * asc_phase_dur)
          { tampy -> PressSpaceElevatorButton ("second");
            asc_autofollow_triggered = true;
          }
        break;

      case ASCPH_ENBLOATEN:
        break;

      case ASCPH_BEFORE_PRESO:
        break;

      case ASCPH_PRESENTATION:
        break;

      case ASCPH_AFTER_PRESO:
        break;

      case ASCPH_ENSVELTEN:
        break;

      case ASCPH_SECOND_RISE:
        if (Tamparams::Current ()->ascension_auto_follow
            &&  ! asc_autofollow_triggered
            &&  asc_phase_stopwatch . CurTimeGlance ()  >  0.45 * asc_phase_dur)
          { tampy -> PressSpaceElevatorButton ("third");
            asc_autofollow_triggered = true;
          }
        if (asc_hvn_incrsn_zeit . CurTimeGlance ()  >=  0.0)
          { if (OeuvreAfterlife *valhalla = RetrieveValhalla ())
              { Ollag *new_coll = new Ollag ("");
                valhalla -> IntroduceNewCollage (new_coll);

                if (ZoftVect *zv_a = ascending_collage -> LocGrapplerZoftVect ())
                  { ZoftVect *zv_b = new_coll -> LocGrapplerZoftVect ();
                    InterpVect smooven (*zv_a, *zv_b);
                    smooven . SetInterpFunc (InterpFuncs::LINEAR);
                    smooven . SetInterpTime
                      (0.49 * Tamparams::Current ()->asc_second_rise_time);
                    ascending_collage -> InstallLocGrapplerZoft (smooven);
                  }
              }
            asc_hvn_incrsn_zeit . SetTimeFlowRate (-1.0);
            asc_hvn_incrsn_zeit . SetTime (-1.0);
          }
        break;

      default:
        break;
    }
}


f64 Orksur::DurationFromAscensionPhase (i64 asc_phas)
{ Tamparams *tam = Tamparams::Current ();
  switch (asc_phas)
    { case ASCPH_TABLE_SLIDE:
        return tam->asc_table_slide_time;
      case ASCPH_FIRST_RISE:
        return tam->asc_first_rise_time;
      case ASCPH_ENBLOATEN:
        return tam->asc_enbloaten_time;
      case ASCPH_BEFORE_PRESO:
        return tam->asc_before_preso_hold_time;
      case ASCPH_PRESENTATION:
        return tam->asc_presentation_time;
      case ASCPH_AFTER_PRESO:
        return tam->asc_after_preso_hold_time;
      case ASCPH_ENSVELTEN:
        return tam->asc_ensvelten_time;
      case ASCPH_SECOND_RISE:
        return tam->asc_second_rise_time;
      case ASCPH_ENTER_HEAVEN:
        return tam->asc_enter_heaven_time;
      default:
        break;
    }
  return -1.0;
}


void Orksur::EffectNextAscensionPhase ()
{ Tamparams *tam = Tamparams::Current ();
  AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes;

  ++ascension_phase;
  f64 asc_prev_phase_dur = asc_phase_dur;
  asc_phase_dur = DurationFromAscensionPhase (ascension_phase);
  asc_phase_stopwatch . ZeroTime ();
  asc_autofollow_triggered = false;

  switch (ascension_phase)
    { case ASCPH_TABLE_SLIDE:
        { asc_table_slide . SetHard (Loc ());
          asc_table_slide . Set (Loc () + Height () * Up ());
          asc_table_slide . SetInterpTime
            (asc_phase_dur);
          asc_table_slide . SetInterpFunc (InterpFuncs::ASYMP_A);
          ascending_collage -> InstallLocGrapplerZoft (asc_table_slide);
          ascending_collage -> AlignToMaes (this);
          AppendChild (ascending_collage);
          if (associated_cinelib)
            { associated_cinelib->fader
                . SetInterpTime (asc_phase_dur);
              associated_cinelib->fader . Set (ZeColor (1.0, 0.1));

              if (AtomicFreezone *afz = Tamglobals::Only ()->sterngerlach)
                { afz->dim_axis.pnt = associated_wallmaes -> Loc ();
                  afz->dim_axis.dir = associated_wallmaes -> Up ();
                  afz->dim_rad . SetInterpFunc (InterpFuncs::QUADRATIC_AB);
                  afz->dim_rad
                    . SetInterpTime (tam->asc_table_slide_time
                                     +  0.9 * tam->asc_first_rise_time);
                  afz->dim_rad . SetHard (0.0);
                  afz->dim_rad . Set (0.4 * associated_wallmaes -> Width ());
                }
            }
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_table_slide_audio, moid);
              sherm -> SendFadeOut ();
            }
          break;
        }

      case ASCPH_FIRST_RISE:
        { const PlatonicMaes *ma = associated_wallmaes;
          ascending_collage -> AlignToMaes (ma);
          ascending_collage -> ScaleZoft ()
            . SetHard (2.5 * ascending_collage -> CurScale ());
          Node *amok = RetrieveValhalla () -> AmokField ();
          amok -> AppendChild (ascending_collage);
          ascending_collage -> InstallLocGrapplerZoft (asc_first_rise);
          Vect s = ascending_collage -> CurScale ();
          ascending_collage -> InstallScaleGrapplerZoft (asc_jump_scale);
          asc_jump_scale . SetInterpTime (0.15 * asc_phase_dur);
          asc_jump_scale . SetHard (0.005 * s);
          asc_jump_scale . Set (s);
          Vect tabtop = Loc () + 0.5 * Height () * Up ();
          Vect wallstart
            = G::PointOntoPlaneProjection (tabtop, ma -> Loc (), ma -> Norm ());
          asc_first_rise . SetInterpFunc (InterpFuncs::QUADRATIC_AB);
          asc_first_rise . SetInterpTime (asc_phase_dur);
          asc_first_rise . SetHard (wallstart);
          asc_first_rise . Set (ma -> Loc ()
                                +  (Tamparams::Current ()->asc_preso_vert_pos
                                    - 0.5) * ma -> Height () * ma -> Up ());
          asc_slipcover -> ScaleZoft () . Set (Vect (Width ()));
          ascending_collage -> AppendChild (asc_slipcover);
          asc_coll_fader . SetHard (ZeColor (1.0, 0.0));
          asc_covr_fader . SetHard (ZeColor (1.0, 1.0));
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_first_rise_audio, moid);
            }
          break;
       }

      case ASCPH_ENBLOATEN:
        { Vect s = ascending_collage -> CurScale ();
          asc_perf_bloat . SetHard (Vect (s));
          asc_perf_bloat . Set (s * 1.75);
          asc_perf_bloat . SetInterpTime (asc_phase_dur);
          asc_perf_bloat . SetInterpFunc (InterpFuncs::QUADRATIC_AB);
          ascending_collage -> InstallScaleGrapplerZoft (asc_perf_bloat);
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_enbloaten_audio, moid);
            }
          break;
        }

      case ASCPH_BEFORE_PRESO:
        { asc_hold_zeit . ZeroTime ();
          asc_coll_fader . SetInterpFunc (InterpFuncs::LINEAR);
          asc_covr_fader . SetInterpFunc (InterpFuncs::LINEAR);
          asc_coll_fader . SetInterpTime (0.9 * asc_phase_dur);
          asc_covr_fader . SetInterpTime (0.6 * asc_phase_dur);
          asc_coll_fader . Set (ZeColor (1.0, 1.0));
          asc_covr_fader . Set (ZeColor (1.0, 0.0));
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_before_preso_hold_audio, moid);
            }
          break;
        }

      case ASCPH_PRESENTATION:
        { asc_perf_zeit . ZeroTime ();
          // and trigger whatever 'performance' bits there may be...
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendFadeIn ();
              sherm -> SendPlaySound (tam->asc_presentation_audio, moid);
            }
          break;
        }

      case ASCPH_AFTER_PRESO:
        { asc_hold_zeit . ZeroTime ();
          ascending_collage -> ExciseChild (asc_slipcover); // save; reuse
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_after_preso_hold_audio, moid);
            }
          break;
        }

      case ASCPH_ENSVELTEN:
        { Vect s = ascending_collage -> CurScale ();
          asc_perf_bloat . Set (s / 1.75);
          asc_perf_bloat . SetInterpTime (asc_phase_dur);
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_ensvelten_audio, moid);
              sherm -> SendFadeOut ();
            }
          break;
        }

      case ASCPH_SECOND_RISE:
        { const PlatonicMaes *ma = associated_wallmaes;
          Swath *sw = RetrieveValhalla () -> SwathFor (ma);
          asc_final_rise . SetInterpFunc (InterpFuncs::QUADRATIC_AB);
          asc_final_rise . SetInterpTime (asc_phase_dur);
          asc_final_rise . SetHard (ascending_collage -> CurLoc ());
          Vect term = sw->plumb . Midpoint () +  0.5 * sw->prone . SpanVect ();
          asc_final_rise . Set (term);
          Vect s = ascending_collage -> CurScale ()
            * tam->coll_scale / tam->coll_reference_scale;
          asc_perf_bloat . SetInterpTime (0.9 * asc_phase_dur);
          asc_perf_bloat . Set (0.85 * s);
          ascending_collage -> InstallLocGrapplerZoft (asc_final_rise);
          asc_hvn_incrsn_zeit . SetTimeFlowRate (1.0);
          asc_hvn_incrsn_zeit . SetTime (-0.50 * asc_phase_dur);
          if (associated_cinelib)
            { associated_cinelib->fader . SetInterpTime (0.895 * asc_phase_dur);
              associated_cinelib->fader . Set (ZeColor (1.0, 1.0));

              if (AtomicFreezone *afz = Tamglobals::Only ()->sterngerlach)
                { afz->dim_rad . SetInterpTime (1.1 * asc_phase_dur);
                  afz->dim_rad . Set (-0.1);
                }
            }
          if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_second_rise_audio, moid);
            }
          break;
        }

      case ASCPH_ENTER_HEAVEN:
        { if (sherm)
            { i64 moid = ZeMonotonicID ();
              sherm -> SendPlaySound (tam->asc_enter_heaven_audio, moid);
              sherm -> SendCleanSlate ();
            }
          ConcludeAscension ();
          break;
        }

      default:
        { fprintf (stderr, "WHAMMO!\nWHAMMO!\nWHAMMO!\nWHAMMO!\n");
          break;
        }
    }
}



Jigglegon *Orksur::FurnishFreeJiggler ()
{ Jigglegon *jig = NULL;
  if (fallow_jigs . size ()  <  1)
    jig = new Jigglegon;
  else
    { jig = fallow_jigs . back ();
      fallow_jigs . pop_back ();
    }

  //  active_jigs . push_back (jig);
  return jig;
}

void Orksur::ReturnJigglerToShelf (Jigglegon *jig)
{ if (! jig)
    return;

  // if (std::find (active_jigs . begin (), active_jigs . end (), jig)
  //     ==  active_jigs . end ())
  //   { // well, that just ain't right. here's your opportunity to fix it!
  //     throw;
  //   }

  if (Node *par = jig -> Parent ())
    par -> ExciseChild (jig);
  fallow_jigs . push_back (jig);
  //  active_jigs . pop_back ();
}


void Orksur::AtomicFirstStrike (Ticato *tic)
{ if (! tic)
    return;
  if (soncho)
    soncho -> InitiateAtomicContact (tic);

  if (! tic->aura)
    tic -> OutfitWithAura (FurnishFreeJiggler ());
  tic->aura->fadist . SetHard (ZeColor (1.0, 1.0));
}

void Orksur::AtomicFinalGutter (Ticato *tic)
{ if (! tic)
    return;
  if (tic->aura)
    tic->aura->fadist . Set (ZeColor (1.0, 0.0));
}



Ticato *Orksur::ClosestAtom (const Vect &p)
{ Ticato *ct = NULL;
  f64 d, cd = CHRM_MAX_F64;

  for (Ticato *tic  :  players)
    if (tic)
      if ((d = tic -> CurLoc () . DistFrom (p))  <  cd)
        { ct = tic;  cd = d; }
  return ct;
}


void Orksur::DistinguishManipulees ()
{ static ZeColor fullc (1.0, 1.0);
  static ZeColor fadec (0.4, 1.0);

  bool never_mind = (hoverees . size ()  ==  0);
  for (Ticato *tic  :  players)
    if (tic)
      tic->re -> SetAdjColor (never_mind ? fullc : fullc); //fadec);

  for (auto &f  :  graspees)
    if (f.second.tic)
      f.second.tic->re -> SetAdjColor (ZeColor (2.0, 1.0));
}


bool Orksur::AppendAtomToCollage (Ticato *tic)
{ if (! tic)
    return false;

  auto it = std::find (players . begin (), players . end (), tic);
  if (it  !=  players . end ())
    { // again, something plentifully wrong; why's it already here?
      assert (2 == 3);
    }

  stringy_list extant_atoms = CollageAtomsNameList ();

  assembly -> AppendChild (tic);  // excises from former parent, see?
  players . push_back (tic);
  Tamparams *tam = Tamparams::Current ();
  tic->accom_sca
    . Set (tic -> BornViaFlickPluck ()
           ?  Vect (tam->grauman_flick_reference_wid
                    / tam->grauman_flick_wid
                    * tam->table_flmatom_scale_factor)
           :  Vect (tam->table_escatom_scale_factor
                    / tam->escatom_wall_scale));

  if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
    { sherm -> SendPlayBoop (5);
      i64 disc_id = ZeMonotonicID ();
      awaiting_audio_sooth[disc_id] = tic;
      sherm -> SendGetSuggestions
        (extant_atoms, tic -> AtomName (),
         (tic->atom_info ? tic->atom_info->duration : 0.0),
         disc_id);
    }

  if (soncho)
    soncho -> InitiateAtomicContact (tic);

  return true;
}


bool Orksur::RemoveAtomFromCollage (Ticato *tic)
{ if (! tic)
    return false;
  FindErase (players, tic);
  tic -> Parent () -> RemoveChild (tic);
  return true;
}


void Orksur::SilenceAllAtoms ()
{ for (Ticato *tic  :  players)
    tic -> SonoSilence ();
}


AtomThusness Orksur::ImpelFreeAtom (Ticato *tic, f64 dt)
{ if (! tic)
    return AtomThusness::CONT;

  Vect newp = tic -> CurLoc ()  +  dt * tic->shov_vel;
  if (G::PointRectContainment (newp, Loc (), Over (), Up (),
                               Width (), Height (), NULL))
    { tic -> LocZoft () . SetHard (newp);
      return AtomThusness::CONT;
    }

  // we're off the edge of the table; project to wall.
  Vect hit;
  if (! G::RayPlaneIntersection (tic -> CurLoc (), tic->shov_vel,
                                 associated_wallmaes -> Loc (),
                                 associated_wallmaes -> Norm (), &hit))
    { // off the table in a direction that doesn't hit a wall
      return AtomThusness::NUKE;
    }

  tic -> LocZoft () . SetHard (hit);
  auto it = std::find (players . begin (), players . end (), tic);
  if (it  ==  players . end ())
    { // well, that's if not impossible then certaianly horrible.
      assert ('a' == 'z');
    }
  players . erase (it);

  ZEBulletinEvent *bev = new ZEBulletinEvent ("atom-deposit");
  bev -> AppendObjPhrase ("inbound-atom", tic);
  bev -> AppendObjPhrase ("from-zone", this);
  bev -> AppendObjPhrase ("onto-maes", associated_wallmaes);
  GraphicsApplication::GetApplication () -> GetSprinkler () . Spray (bev);
  delete bev;

  return AtomThusness::CONT;
}


void Orksur::DisposeOfCollage ()
{ Vect o = Over ();
  Vect u = Up ();
  f64 halfang;
  Vect hit;
  SilenceAllAtoms ();
  if (associated_wallmaes
      &&  G::RayPlaneIntersection (Loc (), u, associated_wallmaes -> Loc (),
                                   associated_wallmaes -> Norm (), &hit))
    halfang = atan2 (0.9 * 0.5 * associated_wallmaes -> Width (),
                     Loc () . DistFrom (hit));
  else
    halfang = M_PI / 180.0 * 30.0;
  for (Ticato *tic  :  players)
    { f64 ang = 2.0 * (drand48 () - 0.5) * halfang;
      f64 spd = Tamparams::Current ()->disposal_speed_threshold
        * (1.0 + drand48 ());
      tic->shov_vel = spd * (cos (ang) * u  +  sin (ang) * o);
    }
}

void Orksur::EliminateCollage ()
{ for (Ticato *tic  :  players)
    { if (Node *par = tic -> Parent ())
        par -> RemoveChild (tic);
      else
        delete tic;
    }
  players . clear ();
  hoverees . clear ();
  graspees . clear ();
}



void Orksur::WhisperSplortily (const std::string &prv, const Vect &proj, f64 dst)
{ Splort *s;
  try  { s = splorts . at (prv); }
  catch (const std::out_of_range &e)
    { AppendChild (s = new Splort);
      splorts[prv] = s;
    }
  s->loc . Set (proj);

  Vect bloff = proj - CornerBL ();
  Vect troff = CornerTR () - proj;
  s->re -> ClearAllLines ();

  const Vect &o = ovr.val, &u = upp.val;
  f64 l = o . Dot (bloff), r = o . Dot (troff);
  if (l < 0.0  ||  r < 0.0)
    return;
  f64 b = u . Dot (bloff), t = u . Dot (troff);
  if (b < 0.0  ||  t < 0.0)
    return;

  s->re -> AppendLine (proj - l * o, proj + r * o);
  s->re -> AppendLine (proj - b * u, proj + t * u);
  float aa = (dst - contact_dist) / (sentient_dist - contact_dist);
  if (aa < 0.0)
    aa = 0.0;
  else if (aa > 1.0)
    aa = 1.0;
  s->re -> SetLinesColor (ZeColor (1.0, 0.5 * (1.0 - aa)));
}


i64 Orksur::ConfectSpatialPointingFromPosition (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  const std::string &prv = e -> Provenance ();
  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 t = n . Dot (p - loc);
  Vect proj = p  -  t * n;

  // ensure that we're within the airspace of the surface.
  // note: may want to restrict this check to harden...
  f64 w = 0.5 * wid.val;
  f64 h = 0.5 * hei.val;
  Vect locl = proj - loc.val;
  f64 u = locl . Dot (ovr.val);
  if (u < -w  ||  u > w)
    return 0;
  if ((u = locl . Dot (upp . val))  <  -h  ||  u > h)
    return 0;

  if (prev_prox_by_prov . find (prv)  ==  prev_prox_by_prov . end ())
    prev_prox_by_prov[prv] = 2.0 * sentient_dist;
  f64 pprox = prev_prox_by_prov[prv];

  if (t >  sentient_dist)
    { // perhaps start a timer whose runout triggers de-hovering etc.?
      if (pprox  <=  sentient_dist)
        {  // spatial-vanish event
        }
      prev_prox_by_prov[prv] = t;
      return 0;
    }

  static std::string syn_s = "synth-";
  std::string newprv = syn_s + prv;

  if (pprox  >  sentient_dist)
    {  // spatial-appear event
    }

  // no matter what at this point we needs must emit a move event
  ZESpatialMoveEvent zsme (newprv, e -> Loc (), -Norm (), Over ());
  zsme . SetForebearEvent (e);
  // now the big question: do we route through general event distribution,
  // or just send to ourselves? maybe the latter for now?
  ZESpatialMove (&zsme);

  if (t  <=  contact_dist)
    { if (pprox  >  contact_dist)  // this here's the harden case
        { ZESpatialHardenEvent zshe (zsme, 0);
          zshe . SetForebearEvent (e);
          ZESpatialHarden (&zshe);
        }
    }
  else  // that is: we're hover -- between harden and oblivion
    { if (pprox  <=  contact_dist)  // soften, wouldn't you know...
        { ZESpatialSoftenEvent zsse (zsme, 0);
          zsse . SetForebearEvent (e);
          ZESpatialSoften (&zsse);
        }
    }
  prev_prox_by_prov[prv] = t;
  return 1;
}


i64 Orksur::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  if (e -> Aim () . Norm () . Dot (Norm ())  >  -0.99)
    return ConfectSpatialPointingFromPosition (e);

  const std::string &prv = e -> Provenance ();

  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 tt = n . Dot (p - loc);
  Vect proj = p  -  tt * n;
  //  tt = fabs (tt);

  // constrain to some reasonable laterally-bounded airspace o'er the table
  if (! G::PointRectContainment (proj, Loc (), Over (), Up (),
                                 1.1 * Width (), 1.1 * Height ()))
    return 0;

  WhisperSplortily (prv, proj, tt);

  auto heff = hoverees . find (prv);
  auto geff = graspees . find (prv);
  assert (heff == hoverees . end ()  ||  geff == graspees . end ());

  i64 sponse;
  // we'll give the sonochoosist first dibs
  if (soncho)
    if (sponse = (soncho -> ZESpatialMove (e))  >  0)
      return sponse;

  if (geff  !=  graspees . end ())  // we're dragging some atom around
    { Vect newp = proj + geff->second.gropoff;
      f64 dt = GraphicsApplication::GetFrameTime () -> GetCurrentDelta ();
      geff->second.tic->shov_vel
        = Vect::zerov;
      geff->second.tic -> LocZoft () . Set (newp);
      return 1;
    }

  Ticato *ca = ClosestAtom (proj);
  if (heff  ==  hoverees . end ())
    { if (ca)
        hoverees[prv] = { ca, Vect::zerov };
    }
  else if (heff->second.tic  !=  ca)
    { // lighting change for outgoing?
      heff->second.tic = ca;
    }

  //  DistinguishManipulees ();
  return 0;
}


i64 Orksur::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (! e)
    return -1;

  // let's reject if not in our airspace
  Vect proj;
  if (! G::PointRectContainment (e -> Loc (), Loc (), Over (), Up (),
                                 Width (), Height (), &proj))
    return 0;

  i64 sponse;
  // as ever, sonochoosist goes first
  if (soncho)
    if (sponse = (soncho -> ZESpatialHarden (e))  >  0)
      return sponse;

  const std::string &prv = e -> Provenance ();
  auto heff = hoverees . find (prv);
  if (heff  ==  hoverees . end ())
    return 0;  // anythig else to do besides, you know, nothing?

  Ticato *tic = heff->second.tic;
  hoverees . erase (heff);

  if (graspees . find (prv)  !=  graspees . end ())
    { // it shouldn't be possible... and yet... well, let's figure it out:
      assert (true == false);
    }

  graspees[prv] = { tic, (tic -> CurLoc () - proj) };
  assembly -> MakeChildLast (tic);
  tic->shov_vel = Vect::zerov;
  tic->interp_adjc . Set (ZeColor (2.0, 1.0));
  AtomicFirstStrike (tic);
  // and also... light the lucky atom up? or like that?
  return 1;
}


i64 Orksur::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (! e)
    return -1;

  i64 sponse;
  // once more: sonochoosist jumps the line
  if (soncho)
    if (sponse = (soncho -> ZESpatialSoften (e))  >  0)
      return sponse;

  const std::string &prv = e -> Provenance ();
  auto geff = graspees . find (prv);
  if (geff  ==  graspees . end ())
    return 0;  // really just that simple?

  if (hoverees . find (prv)  !=  hoverees . end ())
    {  // the heck?
      assert (0.1  ==  1.0);
    }

  hoverees[prv] = geff->second;
  graspees . erase (geff);
  geff->second.tic->interp_adjc . Set (ZeColor (1.0, 1.0));
  AtomicFinalGutter (geff->second.tic);
  /*
    if (ca->shov_vel . Mag ()
    >=  Tamparams::Current ()->disposal_speed_threshold)
    { fprintf (stderr, "PREYZDE LAWD -- <%p> gon' scoot to die!", ca);
    // and but then as well... what.
    // QQQ ca->shov_vel = Vect::zerov;
    }
  */
  return 0;
}


i64 Orksur::ZEYowlAppear (ZEYowlAppearEvent *e)
{ if (! e)
    return -1;
  const std::string &utt = e -> Utterance ();
  const char *ustr = utt . c_str ();
  if (*ustr  ==  '\0')
    return 0;

  char uch = *ustr;
  if (uch >= '0'  &&  uch <= '9')
    { i64 ind = uch - '0';
      if (ind  <  players . size ())
        sel_atom = players[ind];
    }
  else if (uch  ==  'Z')
    { if (sel_atom)
        sel_atom -> SonoSilence ();
    }
  else if (uch >= 'A'  &&  uch <= 'J')
    { i64 ind = uch - 'A';
      if (sel_atom)
        sel_atom -> EnunciateNthSonoOption (ind);
    }
  else if (utt  ==  "^")
    { if (! CurrentlyAscending ())
        EffectAscension ();
    }
  else if (utt  ==  "[")
    { if (soncho)  soncho -> Furl (); }
  else if (utt  ==  "]")
    { if (soncho)  soncho -> Unfurl (); }
  else if (utt  ==  "/")
    { DisposeOfCollage (); }
  else if (utt  ==  "?")
    { EliminateCollage (); }
  else if (utt  ==  ".")
    { SilenceAllAtoms (); }
  else
    return 0;

  return 1;
}

i64 Orksur::ZEBulletin (ZEBulletinEvent *e)
{ if (! e)
    return -1;
  ZEBObjPhrase *phr;
  Ticato *tic;

  if (e -> Says ("drag-maes-change"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("dragee")))
        if (e -> ObjByTag ("to-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  ==  inchoates . end ())
              { inchoates . push_back (tic);
                Tamparams *tam = Tamparams::Current ();
                tic->accom_sca . Set
                  (tic -> BornViaFlickPluck ()
                   ?  Vect (tam->grauman_flick_reference_wid
                            / tam->grauman_flick_wid
                            * tam->table_flmatom_scale_factor)
                   :  Vect (tam->table_escatom_scale_factor
                            / tam->escatom_wall_scale));
              }
          }
        else if (e -> ObjByTag ("from-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  !=  inchoates . end ())
              { inchoates . erase (it);
                tic->accom_sca . Set (Vect (1.0));
              }
          }
    }
  else if (e -> Says ("atom-deposit"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("inbound-atom")))
        if (e -> ObjByTag ("onto-maes")  ==  underlying_maes)
          { AppendAtomToCollage (tic);
            tic->wander_vel = Vect::zerov;
            //tic -> BBoxSetColor (Tamglobals::Only ()->tabatom_bbox_color);
            tic -> OutfitWithAura (FurnishFreeJiggler ());
            if (soncho  &&  soncho -> Active ())
              tic -> FlashAura ();
            auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  !=  inchoates . end ())
              inchoates . erase (it);
          }
    }
  return 0;
}


i64 Orksur::TMPControl (TMPControlEvent *e)
{ if (! e)
    return -1;

  const std::string &pth = e -> Path ();
  if (pth  ==  "/taclient/control")
    { const nl::json &emm = e -> Message ();
      const nl::json &mess
        = (emm . is_array ())  ?  emm . at (0)  :  emm;
      auto blort1 = mess . find ("target");
      auto blort2 = mess . find ("action");
      if (blort1 == mess . end ()  ||  blort2 == mess . end ())
        { fprintf (stderr, "HELL. MANGLED MESSAGE IN Orksur::TASMessage()\n"
                   "... like this: \n%s\n", mess . dump (1) . c_str ());
          return 0;
        }
      const std::string &trgt = blort1 -> get <std::string> ();
      if (trgt  !=  TableName ())
        return 0;
      const std::string &actn = blort2 -> get <std::string> ();
      if (actn  ==  "trigger_ascension")
        { fprintf (stderr, "HEY! HEY THERE! JUST GOT AN ASCENSION MESSAGE FOR "
                   "<%s>...\n", trgt . c_str ());
          if (! CurrentlyAscending ())
            EffectAscension ();
          return 1;
        }
      else if (actn  ==  "trigger_descension")
        { Tamglobals::Only ()->solo_tamp -> PressSpaceElevatorButton ("first");
          return 1;
        }
    }
  else
    { fprintf (stderr, "Orksur::TASMessage() -- unknown OSC 'path': <%s>\n",
               pth . c_str ());
    }
  return 0;
}


i64 Orksur::TASSuggestion (TASSuggestionEvent *e)
{ if (! e)
    return -1;

  i64 disc_id = e -> DiscussionID ();
  auto awaiter = awaiting_audio_sooth . find (disc_id);
  if (awaiter  ==  awaiting_audio_sooth . end ())
    return 0;
  Ticato *tic = awaiter->second;
  awaiting_audio_sooth . erase (awaiter);

  stringy_list suggs = e -> SuggestionNames ();
  fprintf (stderr,"WEEEEELLLLLP. Just got %d audio suggestions:\n",suggs.size());
  for (auto &ess  :  suggs)
    fprintf (stderr,"\"%s\", ",ess.c_str());
  fprintf (stderr,"\n");
  if (! tic)
    return -1;
  tic->sono_options = suggs;
  // and presumably quite a bit more action...
  // if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
  //   if (suggs . size ()  >  0)
  //     { i32 ind = drand48 () * suggs . size ();
  //       fprintf(stderr,"about to request %d = [%s]...\n",ind,suggs[ind].c_str());
  //       sherm -> SendPlaySound (suggs . at (ind));
  //     }
  return 0;
}



i64 Orksur::Inhale (i64 ratch, f64 thyme)
{ if (CurrentlyAscending ())
    { IndulgeAscensionInterstitials ();
      if (AscensionPhaseJustNowDone ())
        EffectNextAscensionPhase ();
    }

  std::vector <Ticato *> *mort = NULL;
  f64 dt = GraphicsApplication::GetFrameTime () -> GetCurrentDelta ();
  for (Ticato *tic  :  players)
    { if (! tic)
        continue;
//
/// the following (which seeks to re-use jigglers) shows up buggy
/// behavior in which some percentage of the time you don't get a
/// jiggle-halo when clicking on a table-atom... no time to figure
/// it out right now.
//
      // if (tic->aura  &&  tic->aura -> FullyFaded ())
      //   { ReturnJigglerToShelf (tic->aura);
      //     tic->aura = NULL;
      //   }
      if (tic->shov_vel . AutoDot ()  >  0.0
          &&  ! AtomIsGrasped (tic))
        if (ImpelFreeAtom (tic, dt)  ==  AtomThusness::NUKE)
          { if (! mort)
              mort = new std::vector <Ticato *> ();
            mort -> push_back (tic);
          }
    }

  if (mort)
    { for (Ticato *mortic  :  *mort)
        { auto it = std::find (players . begin (), players . end (), mortic);
          if (it  ==  players . end ())
            {  // whuh?
              assert (M_PI == 3.0);
            }
          players . erase (it);
        }
      delete mort;
    }

  return 0;
}
