WandExport void DrawSetStrokeAntialias(DrawingWand *wand,
  const MagickBooleanType stroke_antialias)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias=stroke_antialias;
      (void) MVGPrintf(wand,"stroke-antialias %i\n",stroke_antialias != 0 ?
        1 : 0);
    }
}