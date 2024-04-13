WandExport void DrawSetTextAntialias(DrawingWand *wand,
  const MagickBooleanType text_antialias)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias=text_antialias;
      (void) MVGPrintf(wand,"text-antialias %i\n",text_antialias != 0 ? 1 : 0);
    }
}