MagickExport void *ResizeQuantumMemory(void *memory,const size_t count,
  const size_t quantum)
{
  size_t
    size;

  if ((HeapOverflowSanityCheckGetSize(count,quantum,&size) != MagickFalse) ||
      (size > GetMaxMemoryRequest()))
    {
      errno=ENOMEM;
      memory=RelinquishMagickMemory(memory);
      return(NULL);
    }
  return(ResizeMagickMemory(memory,size));
}