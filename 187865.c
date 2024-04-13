WandExport void DrawCircle(DrawingWand *wand,const double ox,const double oy,
  const double px,const double py)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"circle %.20g %.20g %.20g %.20g\n",ox,oy,px,py);
}