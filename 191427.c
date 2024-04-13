static inline void *AcquireAlignedMemory_WinAPI(const size_t size)
{
  return(_aligned_malloc(size,CACHE_LINE_SIZE));
}