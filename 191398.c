static inline void *AcquireAlignedMemory_Generic(const size_t size)
{
  size_t
    extent;

  void
    *memory,
    *p;

  #if SIZE_MAX < ALIGNMENT_OVERHEAD
    #error "CACHE_LINE_SIZE is way too big."
  #endif
  extent=(size+ALIGNMENT_OVERHEAD);
  if (extent <= size)
    {
      errno=ENOMEM;
      return(NULL);
    }
  p=AcquireMagickMemory(extent);
  if (p == NULL)
    return(NULL);
  memory=adjust(p);
  *pointer_to_space_for_actual_base_address(memory)=p;
  return(memory);
}