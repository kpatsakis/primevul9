static const Quantum *GetVirtualPixelStream(const Image *image,
  const VirtualPixelMethod magick_unused(virtual_pixel_method),const ssize_t x,
  const ssize_t y,const size_t columns,const size_t rows,
  ExceptionInfo *exception)
{
  CacheInfo
    *cache_info;

  MagickBooleanType
    status;

  MagickSizeType
    number_pixels;

  size_t
    length;

  magick_unreferenced(virtual_pixel_method);

  /*
    Validate pixel cache geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((x < 0) || (y < 0) ||
      ((x+(ssize_t) columns) > (ssize_t) image->columns) ||
      ((y+(ssize_t) rows) > (ssize_t) image->rows) ||
      (columns == 0) || (rows == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),StreamError,
        "ImageDoesNotContainTheStreamGeometry","`%s'",image->filename);
      return((Quantum *) NULL);
    }
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  number_pixels=(MagickSizeType) columns*rows;
  length=(size_t) number_pixels*cache_info->number_channels*sizeof(Quantum);
  if (cache_info->number_channels == 0)
    length=number_pixels*sizeof(Quantum);
  if (cache_info->metacontent_extent != 0)
    length+=number_pixels*cache_info->metacontent_extent;
  if (cache_info->pixels == (Quantum *) NULL)
    {
      cache_info->length=length;
      status=AcquireStreamPixels(cache_info,exception);
      if (status == MagickFalse)
        {
          cache_info->length=0;
          return((Quantum *) NULL);
        }
    }
  else
    if (cache_info->length < length)
      {
        RelinquishStreamPixels(cache_info);
        cache_info->length=length;
        status=AcquireStreamPixels(cache_info,exception);
        if (status == MagickFalse)
          {
            cache_info->length=0;
            return((Quantum *) NULL);
          }
      }
  cache_info->metacontent=(void *) NULL;
  if (cache_info->metacontent_extent != 0)
    cache_info->metacontent=(void *) (cache_info->pixels+number_pixels*
      cache_info->number_channels);
  return(cache_info->pixels);
}