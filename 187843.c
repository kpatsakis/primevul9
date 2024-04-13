WandExport void DrawSetFontStyle(DrawingWand *wand,const StyleType style)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->style != style))
    {
      CurrentContext->style=style;
      (void) MVGPrintf(wand, "font-style '%s'\n",CommandOptionToMnemonic(
        MagickStyleOptions,(ssize_t) style));
    }
}