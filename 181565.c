static inline void RelinquishStreamPixels(CacheInfo *cache_info)
{
  assert(cache_info != (CacheInfo *) NULL);
  if (cache_info->mapped == MagickFalse)
    (void) RelinquishAlignedMemory(cache_info->pixels);
  else
    (void) UnmapBlob(cache_info->pixels,(size_t) cache_info->length);
  cache_info->pixels=(Quantum *) NULL;
  cache_info->metacontent=(void *) NULL;
  cache_info->length=0;
  cache_info->mapped=MagickFalse;
}