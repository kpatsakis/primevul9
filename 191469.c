static inline void *AcquireAlignedMemory_POSIX(const size_t size)
{
  void
    *memory;

  if (posix_memalign(&memory,CACHE_LINE_SIZE,size))
    return(NULL);
  return(memory);
}