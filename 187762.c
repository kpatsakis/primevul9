WandExport void DrawSetFontStretch(DrawingWand *wand,
  const StretchType font_stretch)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->stretch != font_stretch))
    {
      CurrentContext->stretch=font_stretch;
      (void) MVGPrintf(wand, "font-stretch '%s'\n",CommandOptionToMnemonic(
        MagickStretchOptions,(ssize_t) font_stretch));
    }
}