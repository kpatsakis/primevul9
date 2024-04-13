static inline void *AcquireAlignedMemory_STDC(const size_t size)
{
  size_t
    extent = CACHE_ALIGNED(size);

  if (extent < size)
    {
      errno=ENOMEM;
      return(NULL);
    }
  return(aligned_alloc(CACHE_LINE_SIZE,extent));
}