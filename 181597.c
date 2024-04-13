MagickExport Image *ExcerptImage(const Image *image,
  const RectangleInfo *geometry,ExceptionInfo *exception)
{
#define ExcerptImageTag  "Excerpt/Image"

  CacheView
    *excerpt_view,
    *image_view;

  Image
    *excerpt_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Allocate excerpt image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  excerpt_image=CloneImage(image,geometry->width,geometry->height,MagickTrue,
    exception);
  if (excerpt_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Excerpt each row.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  excerpt_view=AcquireAuthenticCacheView(excerpt_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,excerpt_image,excerpt_image->rows,1)
#endif
  for (y=0; y < (ssize_t) excerpt_image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,geometry->x,geometry->y+y,
      geometry->width,1,exception);
    q=GetCacheViewAuthenticPixels(excerpt_view,0,y,excerpt_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) excerpt_image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait excerpt_traits=GetPixelChannelTraits(excerpt_image,channel);
        if ((traits == UndefinedPixelTrait) ||
            (excerpt_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(excerpt_image,channel,p[i],q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(excerpt_image);
    }
    if (SyncCacheViewAuthenticPixels(excerpt_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ExcerptImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  excerpt_view=DestroyCacheView(excerpt_view);
  image_view=DestroyCacheView(image_view);
  excerpt_image->type=image->type;
  if (status == MagickFalse)
    excerpt_image=DestroyImage(excerpt_image);
  return(excerpt_image);
}