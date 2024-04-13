WandExport char *PixelGetIteratorException(const PixelIterator *iterator,
  ExceptionType *severity)
{
  char
    *description;

  assert(iterator != (const PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  assert(severity != (ExceptionType *) NULL);
  *severity=iterator->exception->severity;
  description=(char *) AcquireQuantumMemory(2UL*MagickPathExtent,
    sizeof(*description));
  if (description == (char *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      iterator->name);
  *description='\0';
  if (iterator->exception->reason != (char *) NULL)
    (void) CopyMagickString(description,GetLocaleExceptionMessage(
      iterator->exception->severity,iterator->exception->reason),MagickPathExtent);
  if (iterator->exception->description != (char *) NULL)
    {
      (void) ConcatenateMagickString(description," (",MagickPathExtent);
      (void) ConcatenateMagickString(description,GetLocaleExceptionMessage(
        iterator->exception->severity,iterator->exception->description),
        MagickPathExtent);
      (void) ConcatenateMagickString(description,")",MagickPathExtent);
    }
  return(description);
}