WandExport void DrawGetStrokeColor(const DrawingWand *wand,
  PixelWand *stroke_color)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(stroke_color != (PixelWand *) NULL);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  PixelSetPixelColor(stroke_color,&CurrentContext->stroke);
}