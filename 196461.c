MagickPrivate cl_mem GetAuthenticOpenCLBuffer(const Image *image,
  MagickCLDevice device,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  assert(device != (const MagickCLDevice) NULL);
  cache_info=(CacheInfo *) image->cache;
  if (cache_info->type == UndefinedCache)
    SyncImagePixelCache((Image *) image,exception);
  if ((cache_info->type != MemoryCache) || (cache_info->mapped != MagickFalse))
    return((cl_mem) NULL);
  LockSemaphoreInfo(cache_info->semaphore);
  if ((cache_info->opencl != (MagickCLCacheInfo) NULL) &&
      (cache_info->opencl->device->context != device->context))
    cache_info->opencl=CopyMagickCLCacheInfo(cache_info->opencl);
  if (cache_info->opencl == (MagickCLCacheInfo) NULL)
    {
      assert(cache_info->pixels != (Quantum *) NULL);
      cache_info->opencl=AcquireMagickCLCacheInfo(device,cache_info->pixels,
        cache_info->length);
    }
  if (cache_info->opencl != (MagickCLCacheInfo) NULL)
    RetainOpenCLMemObject(cache_info->opencl->buffer);
  UnlockSemaphoreInfo(cache_info->semaphore);
  if (cache_info->opencl == (MagickCLCacheInfo) NULL)
    return((cl_mem) NULL);
  assert(cache_info->opencl->pixels == cache_info->pixels);
  return(cache_info->opencl->buffer);
}