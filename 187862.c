WandExport MagickBooleanType DrawSetDensity(DrawingWand *wand,
  const char *density)
{
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",density);
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(density != (const char *) NULL);
  if ((CurrentContext->density == (const char *) NULL) ||
      (wand->filter_off != MagickFalse) ||
      (LocaleCompare(CurrentContext->density,density) != 0))
    {
      (void) CloneString(&CurrentContext->density,density);
      (void) MVGPrintf(wand,"density '%s'\n",density);
    }
  return(MagickTrue);
}