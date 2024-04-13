MagickPrivate Cache AcquirePixelCache(const size_t number_threads)
{
  CacheInfo
    *magick_restrict cache_info;

  char
    *value;

  cache_info=(CacheInfo *) AcquireCriticalMemory(sizeof(*cache_info));
  (void) ResetMagickMemory(cache_info,0,sizeof(*cache_info));
  cache_info->type=UndefinedCache;
  cache_info->mode=IOMode;
  cache_info->colorspace=sRGBColorspace;
  cache_info->file=(-1);
  cache_info->id=GetMagickThreadId();
  cache_info->number_threads=number_threads;
  if (GetOpenMPMaximumThreads() > cache_info->number_threads)
    cache_info->number_threads=GetOpenMPMaximumThreads();
  if (GetMagickResourceLimit(ThreadResource) > cache_info->number_threads)
    cache_info->number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  if (cache_info->number_threads == 0)
    cache_info->number_threads=1;
  cache_info->nexus_info=AcquirePixelCacheNexus(cache_info->number_threads);
  if (cache_info->nexus_info == (NexusInfo **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  value=GetEnvironmentValue("MAGICK_SYNCHRONIZE");
  if (value != (const char *) NULL)
    {
      cache_info->synchronize=IsStringTrue(value);
      value=DestroyString(value);
    }
  value=GetPolicyValue("cache:synchronize");
  if (value != (const char *) NULL)
    {
      cache_info->synchronize=IsStringTrue(value);
      value=DestroyString(value);
    }
  cache_info->semaphore=AcquireSemaphoreInfo();
  cache_info->reference_count=1;
  cache_info->file_semaphore=AcquireSemaphoreInfo();
  cache_info->debug=IsEventLogging();
  cache_info->signature=MagickCoreSignature;
  return((Cache ) cache_info);
}