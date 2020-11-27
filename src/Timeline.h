
#ifndef TIMELINE_MAKES_THE_TRAINS_AND_SPROCKETS_RUN
#define TIMELINE_MAKES_THE_TRAINS_AND_SPROCKETS_RUN


#include "Node.hpp"

#include "TexturedRenderable.hpp"
#include "PolygonRenderable.h"
#include "LinePileRenderable.h"

#include "ZEBulletinEvent.h"
#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"

#include "InterpZoft.h"


using namespace charm;


class SilverScreen;


class Timeline  :  public Zeubject, public Node,
                   public ZESpatialPhagy,
                   public ZEYowlPhagy,
                   public ZEBulletinPhagy
{ public:

  Vect ovr, upp;
  InterpVect shift;
  f64 width, thickth;
  f64 rep_duration;
  f64 play_time;
  f64 hover_time;
  ZoftVect lft_end, rgt_end, upp_hlf, dwn_hlf;
  ZoftVect pnub_pos;
  ZoftVect hnub_pos;
  Node *play_nub;
  Node *hover_nub;
  LinePileRenderable *hover_ren;
  PolygonRenderable *track_rend;

  SilverScreen *cine_receiver;

  Timeline ();
  ~Timeline ()  override;

  void SetOverAndUp (const Vect &o, const Vect &u);

  void SetWidthAndThickth (f64 wi, f64 th);

  SilverScreen *CineReceiver ()
    { return cine_receiver; }
  void EstablishCineReceiver (SilverScreen *essess);

  void SetRepDuration (f64 dur);
  void SetPlayTime (f64 pt);
  void SetHoverTime (f64 ht);

  bool TimeFromSpatialPointing (ZESpatialEvent *e, f64 &out_time);

  i64 ZESpatialMove (ZESpatialMoveEvent *e);
  i64 ZESpatialHarden (ZESpatialHardenEvent *e);
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e);

  i64 ZEYowlAppear (ZEYowlAppearEvent *e);
};


#endif
