WandExport void DrawArc(DrawingWand *wand,const double sx,const double sy,
  const double ex,const double ey,const double sd,const double ed)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"arc %.20g %.20g %.20g %.20g %.20g %.20g\n",sx,sy,ex,
    ey,sd,ed);
}