MagickExport void *AcquireQuantumMemory(const size_t count,const size_t quantum)
{
  size_t
    size;

  if ((HeapOverflowSanityCheckGetSize(count,quantum,&size) != MagickFalse) ||
      (size > GetMaxMemoryRequest()))
    {
      errno=ENOMEM;
      return(NULL);
    }
  return(AcquireMagickMemory(size));
}