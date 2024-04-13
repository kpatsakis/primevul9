static inline MagickBooleanType AcquireStreamPixels(CacheInfo *cache_info,
  ExceptionInfo *exception)
{
  if (cache_info->length != (MagickSizeType) ((size_t) cache_info->length))
    return(MagickFalse);
  cache_info->mapped=MagickFalse;
  cache_info->pixels=(Quantum *) AcquireAlignedMemory(1,(size_t)
    cache_info->length);
  if (cache_info->pixels == (Quantum *) NULL)
    {
      cache_info->mapped=MagickTrue;
      cache_info->pixels=(Quantum *) MapBlob(-1,IOMode,0,(size_t)
        cache_info->length);
    }
  if (cache_info->pixels == (Quantum *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",
        cache_info->filename);
      return(MagickFalse);
    }
  return(MagickTrue);
}