WandExport void DrawSkewX(DrawingWand *wand,const double degrees)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"skewX %.20g\n",degrees);
}