WandExport void DrawGetBorderColor(const DrawingWand *wand,
  PixelWand *border_color)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(border_color != (PixelWand *) NULL);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  PixelSetPixelColor(border_color,&CurrentContext->border_color);
}