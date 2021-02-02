
#include "GraphicsApplication.hpp"
#include "VideoRenderable.hpp"

#include "Orksur.h"

#include "GraumanPalace.h"

#include "Zeubject.h"

#include "ZeEvent.h"

#include <unordered_set>



class Tampo final  :  public GraphicsApplication,
                      public Zeubject,
                      public ZESpatialPhagy, public ZEYowlPhagy
{ public:

  InterpVect elev_transl;
  f64 elev_trans_mult;
  VideoRenderable *steenbeck;
  ch_ptr <AtomicFreezone> freezo;
  ch_ptr <OeuvreAfterlife> vreaft;
  ch_ptr <Orksur> orksu;
  ch_ptr<GraumanPalace> gegyp;
  ch_ptr<GraumanPalace> gchin;
  Node *texxyno;

  // and then: former Sensorium members:
  i32 trig_butt_simulcount;
  u64 trig_butt_ident;
  std::unordered_set <std::string> trig_partic;
  std::unordered_set <std::string> elev_partic;
  bool calibrating;
  bool elevating;
  Vect elev_prevpos;
  std::map <std::string, Vect> recentest_pos;
  ZESpatialPhagy *cally;


  Tampo ();
  ~Tampo ()  override final;

  bool DoWhatThouWilt (i64 ratch, f64 thyme)  override;

  Frontier *IntersectedFrontier (const Vect &frm, const Vect &aim,
                                 Vect *hit_point = NULL);
  void FlatulateCursor (ZESpatialMoveEvent *e);

  void PressSpaceElevatorButton (const std::string floor);

  void AccrueElevatorOffset (const Vect &off);


  // herewith the erstwhile denizens of Sensorium...
  u64 TriggerButtonIdentifier ()  const
    { return trig_butt_ident; }
  void SetTriggerButtonIdentifier (u64 tr_butt)
    { trig_butt_ident = tr_butt; }

  i32 TriggerButtonSimulcount ()  const
    { return trig_butt_simulcount; }
  void SetTriggerButtonSimulcount (i32 cnt)
    { trig_butt_simulcount = cnt; }

  Vect AveragePos ()  const
    { Vect avg;
      i32 cnt = 0;
      for (auto &poo  :  recentest_pos)
        { avg += poo.second;  ++cnt; }
      if (cnt)  avg /= (f64)cnt;
      return avg;
    }

  ZESpatialPhagy *Calibrista ()  const
    { return cally; }
  void SetCalibrista (ZESpatialPhagy *cal)
    { cally = cal; }

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  // see below for the above... can't define inline because uses Tampo...
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;

  i64 ZEYowlAppear (ZEYowlAppearEvent *e)  override;
};
