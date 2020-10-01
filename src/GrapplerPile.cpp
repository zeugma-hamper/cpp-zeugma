
#include "GrapplerPile.h"


namespace charm  {


GrapplerPile::~GrapplerPile ()
{ RemoveAllGrapplers (); }


const Matrix44 &GrapplerPile::PntMat ()  const
{ return pnt_mat; }

const Matrix44 &GrapplerPile::NrmMat ()  const
{ return nrm_mat; }

const Matrix44 &GrapplerPile::InvPntMat ()  const
{ return inv_pnt_mat; }

const Matrix44 &GrapplerPile::InvNrmMat ()  const
{ return inv_nrm_mat; }


i64 GrapplerPile::GrapplerCount ()  const
{ return graps . size (); }

Grappler *GrapplerPile::NthGrappler (i64 ind)  const
{ if (ind < 0  ||  ind >= graps . size ())
    return NULL;
  return graps[ind];
}

i64 GrapplerPile::FindGrappler (Grappler *g)  const
{ if (g)
    { int q = 0;
      for (auto qi = graps . begin ()  ;
           qi != graps . end ()  ;
           ++q, ++qi)
        if (*qi == g)
          return q;
    }
  return -1;
}

Grappler *GrapplerPile::FindGrappler (const std::string &nm)  const
{ for (auto qi = graps . begin ()  ;  qi != graps . end ()  ;  ++qi)
    if (Grappler *gr = *qi)
      if (gr -> Name () == nm)
        return *qi;
  return NULL;
}



GrapplerPile &GrapplerPile::AppendGrappler (Grappler *g)
{ if (g)
    graps . push_back (g);
  return *this;
}

GrapplerPile &GrapplerPile::PrependGrappler (Grappler *g)
{ if (g)
    graps . insert (graps . begin (), g);
  return *this;
}

GrapplerPile &GrapplerPile::InsertGrappler (Grappler *g, i64 ind)
{ if (g  &&  ind >= 0  &&  ind <= graps . size ())
    graps . insert (graps . begin () + ind, g);
  return *this;
}


GrapplerPile &GrapplerPile::RemoveGrappler (Grappler *g)
{ if (! g)
    return *this;
  for (auto qi = graps . begin ()  ;  qi != graps . end ()  ;  ++qi)
    if (*qi == g)
      { graps . erase (qi);
        break;
      }
  return *this;
}

GrapplerPile &GrapplerPile::RemoveNthGrappler (i64 ind)
{ if (ind >= 0  ||  ind < graps . size ())
    graps . erase (graps . begin () + ind);
  return *this;
}

GrapplerPile &GrapplerPile::RemoveAllGrapplers ()
{ graps . clear ();
  pnt_mat . LoadIdent ();  nrm_mat . LoadIdent ();
  inv_pnt_mat . LoadIdent ();  inv_nrm_mat . LoadIdent ();
  return *this;
}


i64 GrapplerPile::Inhale (i64 steppe, f64 thyme)
{ if (GrapplerCount ()  <  1)
    return 0;
  auto qi = graps . begin ();
  Grappler *g = *qi;
  g -> Inhale (steppe, thyme);
  pnt_mat = g -> PntMat ();
  nrm_mat = g -> NrmMat ();
  inv_pnt_mat = g -> InvPntMat ();
  inv_nrm_mat = g -> InvNrmMat ();
  for (++qi  ;  qi != graps . end ()  ;  ++qi)
    { Grappler *g = *qi;
      g -> Inhale (steppe, thyme);
      pnt_mat . MulSelfBy (g -> PntMat ());
      nrm_mat . MulSelfBy (g -> NrmMat ());
      inv_pnt_mat . PreMulSelfBy (g -> InvPntMat ());
      inv_nrm_mat . PreMulSelfBy (g -> InvNrmMat ());
    }
  return 0;
}


}  // you sure you want that third hemlock margarita, namespace charm?
