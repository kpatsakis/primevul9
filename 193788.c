MagickExport void *ResizeMagickMemory(void *memory,const size_t size)
{
  register void
    *block;

  if (memory == (void *) NULL)
    return(AcquireMagickMemory(size));
#if !defined(MAGICKCORE_ANONYMOUS_MEMORY_SUPPORT)
  block=memory_methods.resize_memory_handler(memory,size == 0 ? 1UL : size);
  if (block == (void *) NULL)
    memory=RelinquishMagickMemory(memory);
#else
  LockSemaphoreInfo(memory_semaphore);
  block=ResizeBlock(memory,size == 0 ? 1UL : size);
  if (block == (void *) NULL)
    {
      if (ExpandHeap(size == 0 ? 1UL : size) == MagickFalse)
        {
          UnlockSemaphoreInfo(memory_semaphore);
          memory=RelinquishMagickMemory(memory);
          ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
        }
      block=ResizeBlock(memory,size == 0 ? 1UL : size);
      assert(block != (void *) NULL);
    }
  UnlockSemaphoreInfo(memory_semaphore);
  memory=RelinquishMagickMemory(memory);
#endif
  return(block);
}