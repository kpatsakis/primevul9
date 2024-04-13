WandExport ExceptionType DrawGetExceptionType(const DrawingWand *wand)
{
  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  return(wand->exception->severity);
}