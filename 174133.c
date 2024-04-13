MagickExport MagickBooleanType NegateImage(Image *image,
  const MagickBooleanType grayscale,ExceptionInfo *exception)
{
#define NegateImageTag  "Negate/Image"

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

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class == PseudoClass)
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      /*
        Negate colormap.
      */
      if( grayscale != MagickFalse )
        if ((image->colormap[i].red != image->colormap[i].green) ||
            (image->colormap[i].green != image->colormap[i].blue))
          continue;
      if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].red=QuantumRange-image->colormap[i].red;
      if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].green=QuantumRange-image->colormap[i].green;
      if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
        image->colormap[i].blue=QuantumRange-image->colormap[i].blue;
    }
  /*
    Negate image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
  if( grayscale != MagickFalse )
    {
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        MagickBooleanType
          sync;

        register Quantum
          *magick_restrict q;

        register ssize_t
          x;

        if (status == MagickFalse)
          continue;
        q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
          exception);
        if (q == (Quantum *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          register ssize_t
            j;

          if ((GetPixelWriteMask(image,q) == 0) ||
              IsPixelGray(image,q) != MagickFalse)
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
            q[j]=QuantumRange-q[j];
          }
          q+=GetPixelChannels(image);
        }
        sync=SyncCacheViewAuthenticPixels(image_view,exception);
        if (sync == MagickFalse)
          status=MagickFalse;
        if (image->progress_monitor != (MagickProgressMonitor) NULL)
          {
            MagickBooleanType
              proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp critical (MagickCore_NegateImage)
#endif
            proceed=SetImageProgress(image,NegateImageTag,progress++,
              image->rows);
            if (proceed == MagickFalse)
              status=MagickFalse;
          }
      }
      image_view=DestroyCacheView(image_view);
      return(MagickTrue);
    }
  /*
    Negate image.
  */
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
        q[j]=QuantumRange-q[j];
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
        #pragma omp critical (MagickCore_NegateImage)
#endif
        proceed=SetImageProgress(image,NegateImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}