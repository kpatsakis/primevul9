static MagickBooleanType SetCacheAlphaChannel(Image *image,const Quantum alpha,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  CacheView
    *magick_restrict image_view;

  MagickBooleanType
    status;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  image->alpha_trait=BlendPixelTrait;
  status=MagickTrue;
  image_view=AcquireVirtualCacheView(image,exception);  /* must be virtual */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelAlpha(image,alpha,q);
      q+=GetPixelChannels(image);
    }
    status=SyncCacheViewAuthenticPixels(image_view,exception);
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}