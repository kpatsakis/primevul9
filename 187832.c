WandExport char *DrawGetException(const DrawingWand *wand,
  ExceptionType *severity)
{
  char
    *description;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(severity != (ExceptionType *) NULL);
  *severity=wand->exception->severity;
  description=(char *) AcquireQuantumMemory(2UL*MagickPathExtent,
    sizeof(*description));
  if (description == (char *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      wand->name);
  *description='\0';
  if (wand->exception->reason != (char *) NULL)
    (void) CopyMagickString(description,GetLocaleExceptionMessage(
      wand->exception->severity,wand->exception->reason),
      MagickPathExtent);
  if (wand->exception->description != (char *) NULL)
    {
      (void) ConcatenateMagickString(description," (",MagickPathExtent);
      (void) ConcatenateMagickString(description,GetLocaleExceptionMessage(
        wand->exception->severity,wand->exception->description),
        MagickPathExtent);
      (void) ConcatenateMagickString(description,")",MagickPathExtent);
    }
  return(description);
}