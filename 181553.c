static Quantum *QueueAuthenticPixelsStream(Image *image,const ssize_t x,
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

  StreamHandler
    stream_handler;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) ||
      ((x+(ssize_t) columns) > (ssize_t) image->columns) ||
      ((y+(ssize_t) rows) > (ssize_t) image->rows) ||
      (columns == 0) || (rows == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),StreamError,
        "ImageDoesNotContainTheStreamGeometry","`%s'",image->filename);
      return((Quantum *) NULL);
    }
  stream_handler=GetBlobStreamHandler(image);
  if (stream_handler == (StreamHandler) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),StreamError,
        "NoStreamHandlerIsDefined","`%s'",image->filename);
      return((Quantum *) NULL);
    }
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if ((image->storage_class != GetPixelCacheStorageClass(image->cache)) ||
      (image->colorspace != GetPixelCacheColorspace(image->cache)))
    {
      if (GetPixelCacheStorageClass(image->cache) == UndefinedClass)
        (void) stream_handler(image,(const void *) NULL,(size_t)
          cache_info->columns);
      cache_info->storage_class=image->storage_class;
      cache_info->colorspace=image->colorspace;
      cache_info->columns=image->columns;
      cache_info->rows=image->rows;
      image->cache=cache_info;
    }
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  cache_info->columns=columns;
  cache_info->rows=rows;
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