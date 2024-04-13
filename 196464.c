MagickPrivate Cache ClonePixelCache(const Cache cache)
{
  CacheInfo
    *magick_restrict clone_info;

  const CacheInfo
    *magick_restrict cache_info;

  assert(cache != NULL);
  cache_info=(const CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      cache_info->filename);
  clone_info=(CacheInfo *) AcquirePixelCache(cache_info->number_threads);
  clone_info->virtual_pixel_method=cache_info->virtual_pixel_method;
  return((Cache ) clone_info);
}