WandExport void DrawSetTextInterwordSpacing(DrawingWand *wand,
  const double interword_spacing)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);

  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (fabs((CurrentContext->interword_spacing-
        interword_spacing)) >= MagickEpsilon))
    {
      CurrentContext->interword_spacing=interword_spacing;
      (void) MVGPrintf(wand,"interword-spacing %lf\n",interword_spacing);
    }
}