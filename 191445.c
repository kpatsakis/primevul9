MagickExport void *AcquireCriticalMemory(const size_t size)
{
  void
    *memory;

  /*
    Fail if memory request cannot be fulfilled.
  */
  memory=AcquireMagickMemory(size);
  if (memory == (void *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  return(memory);
}