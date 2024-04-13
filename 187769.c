WandExport char *DrawGetFont(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->font != (char *) NULL)
    return(AcquireString(CurrentContext->font));
  return((char *) NULL);
}