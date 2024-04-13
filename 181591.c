static MagickBooleanType CopyImageRegion(Image *destination,const Image *source,  const size_t columns,const size_t rows,const ssize_t sx,const ssize_t sy,
  const ssize_t dx,const ssize_t dy,ExceptionInfo *exception)
{
  CacheView
    *source_view,
    *destination_view;

  MagickBooleanType
    status;

  ssize_t
    y;

  if (columns == 0)
    return(MagickTrue);
  status=MagickTrue;
  source_view=AcquireVirtualCacheView(source,exception);
  destination_view=AcquireAuthenticCacheView(destination,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(source,destination,rows,1)
#endif
  for (y=0; y < (ssize_t) rows; y++)
  {
    MagickBooleanType
      sync;

    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    /*
      Transfer scanline.
    */
    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(source_view,sx,sy+y,columns,1,exception);
    q=GetCacheViewAuthenticPixels(destination_view,dx,dy+y,columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(source); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(source,i);
        PixelTrait source_traits=GetPixelChannelTraits(source,channel);
        PixelTrait destination_traits=GetPixelChannelTraits(destination,
          channel);
        if ((source_traits == UndefinedPixelTrait) ||
            (destination_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(destination,channel,p[i],q);
      }
      p+=GetPixelChannels(source);
      q+=GetPixelChannels(destination);
    }
    sync=SyncCacheViewAuthenticPixels(destination_view,exception);
    if (sync == MagickFalse)
      status=MagickFalse;
  }
  destination_view=DestroyCacheView(destination_view);
  source_view=DestroyCacheView(source_view);
  return(status);
}