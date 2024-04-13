WandExport char *DrawGetClipPath(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->clip_mask != (char *) NULL)
    return((char *) AcquireString(CurrentContext->clip_mask));
  return((char *) NULL);
}