static inline void *AcquireCriticalMemory(const size_t size)
{
  register void
    *memory;
 
  /*
    Fail if memory request cannot be fulfilled.
  */
  memory=AcquireMagickMemory(size);
  if (memory == (void *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  return(memory);
}