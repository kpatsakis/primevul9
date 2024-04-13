MagickExport MagickBooleanType PerceptibleImage(Image *image,
  const double epsilon,ExceptionInfo *exception)
{
#define PerceptibleImageTag  "Perceptible/Image"

  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        i;

      register PixelInfo
        *magick_restrict q;

      q=image->colormap;
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        q->red=(double) PerceptibleThreshold(ClampToQuantum(q->red),
          epsilon);
        q->green=(double) PerceptibleThreshold(ClampToQuantum(q->green),
          epsilon);
        q->blue=(double) PerceptibleThreshold(ClampToQuantum(q->blue),
          epsilon);
        q->alpha=(double) PerceptibleThreshold(ClampToQuantum(q->alpha),
          epsilon);
        q++;
      }
      return(SyncImage(image,exception));
    }
  /*
    Perceptible image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

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
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        q[i]=PerceptibleThreshold(q[i],epsilon);
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
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,PerceptibleImageTag,progress,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}