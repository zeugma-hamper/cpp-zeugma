
/*
  Node *splat = new Node;
  PolygonRenderable *polysplat = new PolygonRenderable;
  i64 nv = 14;
  SinuVect centz (1000.0 * frnt -> Over (), 0.05, frnt -> Loc ());
  for (i64 q = 0  ;  q < nv  ;  ++q)
    { f64 r = 0.3 * frnt -> Height ();
      f64 theeeta = 2.0 * M_PI / (f64)nv * (f64)q;
      Vect radv = cos (theeeta) * Vect::xaxis  +  sin (theeeta) * Vect::yaxis;
      SinuVect arm (0.3 * r * radv, 0.166 + 0.05 * drand48 (),
                    0.5 * (1.0 + q%2) * r * radv);
      SumVect voit (arm, centz);
      polysplat -> AppendVertex (voit);
    }
  polysplat -> SetFillColor (SinuZoft (ZeColor (0.0, 0.0, 0.5, 0.3), 0.16,
                                       ZeColor (1.0, 1.0, 0.0, 0.3)));
  splat -> AppendRenderable (polysplat);
  polysplat -> SetShouldEdge (true);
//  polysplat -> SetShouldFill (false);
//  polysplat -> SetShouldClose (false);
  g_windshield -> AppendChild (splat);
*/

/*
  splat = new Node (polysplat = new PolygonRenderable);
  f64 jiglet = 0.025 * frnt -> Height ();
  SinuVect jigamp (Vect (jiglet, jiglet, 0.0), 0.2975);
  for (i64 q = 0  ;  q < 12  ;  ++q)
    { f64 r = 0.3 * frnt -> Height ();
      f64 theeeta = 0.5 * M_PI * (f64)q;
      ZoftVect crnr (frnt -> Loc ()
                     +  r * cos (theeeta) * Vect::xaxis
                     +  r * sin (theeeta) * Vect::yaxis);
      RandVect jigv (jigamp, ZoftVect ());
      SumVect voitex (crnr, jigv);
      polysplat -> AppendVertex (voitex);
    }
  splat -> AppendRenderable (polysplat);
  polysplat -> SetShouldEdge (true);
  polysplat -> SetEdgeColor (ZeColor (1.0, 0.25));
  polysplat -> SetShouldFill (false);
//  g_windshield -> AppendChild (splat);
*/

/*
  TextureParticulars tipi
    = CreateTexture2D ("/tmp/SIGN.jpg", DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (tipi);
  texre -> AdjColorZoft ()
    . BecomeLike (SinuColor (ZeColor (0.0, 0.5), 1.0, ZeColor (1.0, 0.5)));
  Node *texno = (tamp.texxyno = new Node (texre));
  texno -> Scale (1300.0);
  texno -> Translate (frnt -> Loc ());
//  g_wallpaper -> AppendChild (texno);
*/
