WandExport void DrawSetBorderColor(DrawingWand *wand,
  const PixelWand *border_wand)
{
  PixelInfo
    *current_border,
    border_color,
    new_border;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(border_wand != (const PixelWand *) NULL);
  PixelGetQuantumPacket(border_wand,&border_color);
  new_border=border_color;
  current_border=(&CurrentContext->border_color);
  if ((wand->filter_off != MagickFalse) ||
      (IsPixelInfoEquivalent(current_border,&new_border) == MagickFalse))
    {
      CurrentContext->border_color=new_border;
      (void) MVGPrintf(wand,"border-color '");
      MVGAppendColor(wand,&border_color);
      (void) MVGPrintf(wand,"'\n");
    }
}