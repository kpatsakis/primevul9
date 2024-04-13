MagickExport void DestroyMagickMemory(void)
{
#if defined(MAGICKCORE_ANONYMOUS_MEMORY_SUPPORT)
  register ssize_t
    i;

  if (memory_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&memory_semaphore);
  LockSemaphoreInfo(memory_semaphore);
  for (i=0; i < (ssize_t) memory_pool.number_segments; i++)
    if (memory_pool.segments[i]->mapped == MagickFalse)
      memory_methods.destroy_memory_handler(
        memory_pool.segments[i]->allocation);
    else
      (void) UnmapBlob(memory_pool.segments[i]->allocation,
        memory_pool.segments[i]->length);
  free_segments=(DataSegmentInfo *) NULL;
  (void) ResetMagickMemory(&memory_pool,0,sizeof(memory_pool));
  UnlockSemaphoreInfo(memory_semaphore);
  RelinquishSemaphoreInfo(&memory_semaphore);
#endif
}