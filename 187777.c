WandExport void DrawSetTextUnderColor(DrawingWand *wand,
  const PixelWand *under_wand)
{
  PixelInfo
    under_color;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(under_wand != (const PixelWand *) NULL);
  PixelGetQuantumPacket(under_wand,&under_color);
  if ((wand->filter_off != MagickFalse) ||
      (IsPixelInfoEquivalent(&CurrentContext->undercolor,&under_color) == MagickFalse))
    {
      CurrentContext->undercolor=under_color;
      (void) MVGPrintf(wand,"text-undercolor '");
      MVGAppendColor(wand,&under_color);
      (void) MVGPrintf(wand,"'\n");
    }
}