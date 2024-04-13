WandExport void DrawEllipse(DrawingWand *wand,const double ox,const double oy,
  const double rx,const double ry,const double start,const double end)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"ellipse %.20g %.20g %.20g %.20g %.20g %.20g\n",ox,oy,
    rx,ry,start,end);
}