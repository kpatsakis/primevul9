MagickExport MagickBooleanType LevelizeImage(Image *image,
  const double black_point,const double white_point,const double gamma,
  ExceptionInfo *exception)
{
#define LevelizeImageTag  "Levelize/Image"
#define LevelizeValue(x) ClampToQuantum(((MagickRealType) gamma_pow((double) \
  (QuantumScale*(x)),gamma))*(white_point-black_point)+black_point)

  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  register ssize_t
    i;

  ssize_t
    y;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class == PseudoClass)
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      /*
        Level colormap.
      */
      if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].red=(double) LevelizeValue(image->colormap[i].red);
      if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].green=(double) LevelizeValue(
          image->colormap[i].green);
      if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].blue=(double) LevelizeValue(image->colormap[i].blue);
      if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].alpha=(double) LevelizeValue(
          image->colormap[i].alpha);
    }
  /*
    Level image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,image,image->rows,1)
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
      register ssize_t
        j;

      if (GetPixelWriteMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      for (j=0; j < (ssize_t) GetPixelChannels(image); j++)
      {
        PixelChannel channel=GetPixelChannelChannel(image,j);
        PixelTrait traits=GetPixelChannelTraits(image,channel);
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        q[j]=LevelizeValue(q[j]);
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_LevelizeImage)
#endif
        proceed=SetImageProgress(image,LevelizeImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}