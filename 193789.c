MagickExport void *AcquireAlignedMemory(const size_t count,const size_t quantum)
{
#define AlignedExtent(size,alignment) \
  (((size)+((alignment)-1)) & ~((alignment)-1))

  size_t
    alignment,
    extent,
    size;

  void
    *memory;

  if (HeapOverflowSanityCheck(count,quantum) != MagickFalse)
    return((void *) NULL);
  memory=NULL;
  size=count*quantum;
  alignment=CACHE_LINE_SIZE;
  if (size > (size_t) (GetMagickPageSize() >> 1))
    alignment=GetMagickPageSize();
  extent=AlignedExtent(size,CACHE_LINE_SIZE);
  if ((size == 0) || (extent < size))
    return((void *) NULL);
#if defined(MAGICKCORE_HAVE_POSIX_MEMALIGN)
  if (posix_memalign(&memory,alignment,extent) != 0)
    memory=NULL;
#elif defined(MAGICKCORE_HAVE__ALIGNED_MALLOC)
  memory=_aligned_malloc(extent,alignment);
#else
  {
    void
      *p;

    extent=(size+alignment-1)+sizeof(void *);
    if (extent > size)
      {
        p=malloc(extent);
        if (p != NULL)
          {
            memory=(void *) AlignedExtent((size_t) p+sizeof(void *),alignment);
            *((void **) memory-1)=p;
          }
      }
  }
#endif
  return(memory);
}