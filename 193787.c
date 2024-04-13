MagickExport void *AcquireMagickMemory(const size_t size)
{
  register void
    *memory;

#if !defined(MAGICKCORE_ANONYMOUS_MEMORY_SUPPORT)
  memory=memory_methods.acquire_memory_handler(size == 0 ? 1UL : size);
#else
  if (memory_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&memory_semaphore);
  if (free_segments == (DataSegmentInfo *) NULL)
    {
      LockSemaphoreInfo(memory_semaphore);
      if (free_segments == (DataSegmentInfo *) NULL)
        {
          register ssize_t
            i;

          assert(2*sizeof(size_t) > (size_t) (~SizeMask));
          (void) ResetMagickMemory(&memory_pool,0,sizeof(memory_pool));
          memory_pool.allocation=SegmentSize;
          memory_pool.blocks[MaxBlocks]=(void *) (-1);
          for (i=0; i < MaxSegments; i++)
          {
            if (i != 0)
              memory_pool.segment_pool[i].previous=
                (&memory_pool.segment_pool[i-1]);
            if (i != (MaxSegments-1))
              memory_pool.segment_pool[i].next=(&memory_pool.segment_pool[i+1]);
          }
          free_segments=(&memory_pool.segment_pool[0]);
        }
      UnlockSemaphoreInfo(memory_semaphore);
    }
  LockSemaphoreInfo(memory_semaphore);
  memory=AcquireBlock(size == 0 ? 1UL : size);
  if (memory == (void *) NULL)
    {
      if (ExpandHeap(size == 0 ? 1UL : size) != MagickFalse)
        memory=AcquireBlock(size == 0 ? 1UL : size);
    }
  UnlockSemaphoreInfo(memory_semaphore);
#endif
  return(memory);
}