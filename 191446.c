static inline void *align_to_cache(void *const p)
{
  return((void *) CACHE_ALIGNED((MagickAddressType) p));
}