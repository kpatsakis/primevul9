WandExport void DrawRoundRectangle(DrawingWand *wand,double x1,double y1,
  double x2,double y2,double rx,double ry)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"roundrectangle %.20g %.20g %.20g %.20g %.20g %.20g\n",
    x1,y1,x2,y2,rx,ry);
}