WandExport void DrawSetTextDirection(DrawingWand *wand,
  const DirectionType direction)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);

  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->direction != direction))
    {
      CurrentContext->direction=direction;
      (void) MVGPrintf(wand,"direction '%s'\n",CommandOptionToMnemonic(
        MagickDirectionOptions,(ssize_t) direction));
    }
}