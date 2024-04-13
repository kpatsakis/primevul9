WandExport void DrawSetFillOpacity(DrawingWand *wand,const double fill_opacity)
{
  double
    alpha;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  alpha=(double) ClampToQuantum(QuantumRange*fill_opacity);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->fill.alpha != alpha))
    {
      CurrentContext->fill.alpha=alpha;
      (void) MVGPrintf(wand,"fill-opacity %.20g\n",fill_opacity);
    }
}