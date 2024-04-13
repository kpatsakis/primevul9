WandExport void DrawSetTextKerning(DrawingWand *wand,const double kerning)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);

  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) &&
      (fabs((CurrentContext->kerning-kerning)) >= MagickEpsilon))
    {
      CurrentContext->kerning=kerning;
      (void) MVGPrintf(wand,"kerning %lf\n",kerning);
    }
}