WandExport void DrawSetFontSize(DrawingWand *wand,const double pointsize)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (fabs(CurrentContext->pointsize-pointsize) >= MagickEpsilon))
    {
      CurrentContext->pointsize=pointsize;
      (void) MVGPrintf(wand,"font-size %.20g\n",pointsize);
    }
}