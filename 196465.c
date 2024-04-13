static void CopyOpenCLBuffer(CacheInfo *magick_restrict cache_info)
{
  assert(cache_info != (CacheInfo *) NULL);
  assert(cache_info->signature == MagickCoreSignature);
  if ((cache_info->type != MemoryCache) ||
      (cache_info->opencl == (MagickCLCacheInfo) NULL))
    return;
  /*
    Ensure single threaded access to OpenCL environment.
  */
  LockSemaphoreInfo(cache_info->semaphore);
  cache_info->opencl=CopyMagickCLCacheInfo(cache_info->opencl);
  UnlockSemaphoreInfo(cache_info->semaphore);
}