WandExport void DrawGetTextUnderColor(const DrawingWand *wand,
  PixelWand *under_color)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(under_color != (PixelWand *) NULL);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  PixelSetPixelColor(under_color,&CurrentContext->undercolor);
}