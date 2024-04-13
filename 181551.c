static MagickBooleanType SyncAuthenticPixelsStream(Image *image,
  ExceptionInfo *exception)
{
  CacheInfo
    *cache_info;

  size_t
    length;

  StreamHandler
    stream_handler;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  stream_handler=GetBlobStreamHandler(image);
  if (stream_handler == (StreamHandler) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),StreamError,
        "NoStreamHandlerIsDefined","`%s'",image->filename);
      return(MagickFalse);
    }
  length=stream_handler(image,cache_info->pixels,(size_t) cache_info->columns);
  return(length == cache_info->columns ? MagickTrue : MagickFalse);
}