WandExport void DrawSetTextAlignment(DrawingWand *wand,
  const AlignType alignment)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->align != alignment))
    {
      CurrentContext->align=alignment;
      (void) MVGPrintf(wand,"text-align '%s'\n",CommandOptionToMnemonic(
        MagickAlignOptions,(ssize_t) alignment));
    }
}