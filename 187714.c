WandExport void DrawSetViewbox(DrawingWand *wand,const double x1,
  const double y1,const double x2,const double y2)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"viewbox %.20g %.20g %.20g %.20g\n",x1,y1,x2,y2);
}