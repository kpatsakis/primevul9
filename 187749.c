WandExport char *DrawGetTextEncoding(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->encoding != (char *) NULL)
    return((char *) AcquireString(CurrentContext->encoding));
  return((char *) NULL);
}