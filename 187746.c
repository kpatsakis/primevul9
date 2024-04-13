WandExport char *DrawGetDensity(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->density != (char *) NULL)
    return((char *) AcquireString(CurrentContext->density));
  return((char *) NULL);
}