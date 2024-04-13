WandExport void DrawGetFillColor(const DrawingWand *wand,PixelWand *fill_color)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(fill_color != (PixelWand *) NULL);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  PixelSetPixelColor(fill_color,&CurrentContext->fill);
}