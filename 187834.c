WandExport void DrawSetStrokeOpacity(DrawingWand *wand,
  const double opacity)
{
  double
    alpha;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  alpha=(double) ClampToQuantum(QuantumRange*opacity);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->stroke.alpha != alpha))
    {
      CurrentContext->stroke.alpha=alpha;
      (void) MVGPrintf(wand,"stroke-opacity %.20g\n",opacity);
    }
}