static void DestroyPixelStream(Image *image)
{
  CacheInfo
    *cache_info;

  MagickBooleanType
    destroy;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  destroy=MagickFalse;
  LockSemaphoreInfo(cache_info->semaphore);
  cache_info->reference_count--;
  if (cache_info->reference_count == 0)
    destroy=MagickTrue;
  UnlockSemaphoreInfo(cache_info->semaphore);
  if (destroy == MagickFalse)
    return;
  RelinquishStreamPixels(cache_info);
  if (cache_info->nexus_info != (NexusInfo **) NULL)
    cache_info->nexus_info=DestroyPixelCacheNexus(cache_info->nexus_info,
      cache_info->number_threads);
  if (cache_info->file_semaphore != (SemaphoreInfo *) NULL)
    RelinquishSemaphoreInfo(&cache_info->file_semaphore);
  if (cache_info->semaphore != (SemaphoreInfo *) NULL)
    RelinquishSemaphoreInfo(&cache_info->semaphore);
  cache_info=(CacheInfo *) RelinquishMagickMemory(cache_info);
}