WandExport char *DrawGetFontFamily(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->family != NULL)
    return(AcquireString(CurrentContext->family));
  return((char *) NULL);
}