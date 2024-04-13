WandExport void DrawSetFillColor(DrawingWand *wand,const PixelWand *fill_wand)
{
  PixelInfo
    *current_fill,
    fill_color,
    new_fill;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(fill_wand != (const PixelWand *) NULL);
  PixelGetQuantumPacket(fill_wand,&fill_color);
  new_fill=fill_color;
  current_fill=(&CurrentContext->fill);
  if ((wand->filter_off != MagickFalse) ||
      (IsPixelInfoEquivalent(current_fill,&new_fill) == MagickFalse))
    {
      CurrentContext->fill=new_fill;
      (void) MVGPrintf(wand,"fill '");
      MVGAppendColor(wand,&fill_color);
      (void) MVGPrintf(wand,"'\n");
    }
}