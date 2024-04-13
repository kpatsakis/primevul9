WandExport void DrawSetStrokeColor(DrawingWand *wand,
  const PixelWand *stroke_wand)
{
  PixelInfo
    *current_stroke,
    new_stroke,
    stroke_color;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(stroke_wand != (const PixelWand *) NULL);
  PixelGetQuantumPacket(stroke_wand,&stroke_color);
  new_stroke=stroke_color;
  current_stroke=(&CurrentContext->stroke);
  if ((wand->filter_off != MagickFalse) ||
      (IsPixelInfoEquivalent(current_stroke,&new_stroke) == MagickFalse))
    {
      CurrentContext->stroke=new_stroke;
      (void) MVGPrintf(wand,"stroke '");
      MVGAppendColor(wand,&stroke_color);
      (void) MVGPrintf(wand,"'\n");
    }
}