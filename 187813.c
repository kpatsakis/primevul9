WandExport void DrawSetTextDecoration(DrawingWand *wand,
  const DecorationType decoration)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->decorate != decoration))
    {
      CurrentContext->decorate=decoration;
      (void) MVGPrintf(wand,"decorate '%s'\n",CommandOptionToMnemonic(
        MagickDecorateOptions,(ssize_t) decoration));
    }
}