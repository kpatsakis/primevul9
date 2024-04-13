MagickExport void *AcquireAlignedMemory(const size_t count,const size_t quantum)
{
  size_t
    size;

  if (HeapOverflowSanityCheckGetSize(count,quantum,&size) != MagickFalse)
    {
      errno=ENOMEM;
      return(NULL);
    }
  if (memory_methods.acquire_aligned_memory_handler != (AcquireAlignedMemoryHandler) NULL)
    return(memory_methods.acquire_aligned_memory_handler(size,CACHE_LINE_SIZE));
  return(AcquireAlignedMemory_Actual(size));
}