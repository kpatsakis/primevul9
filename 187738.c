WandExport void DrawSetFontWeight(DrawingWand *wand,
  const size_t font_weight)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      (void) MVGPrintf(wand,"font-weight %.20g\n",(double) font_weight);
    }
}