MagickExport Image *TransposeImage(const Image *image,ExceptionInfo *exception)
{
#define TransposeImageTag  "Transpose/Image"

  CacheView
    *image_view,
    *transpose_view;

  Image
    *transpose_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  RectangleInfo
    page;

  ssize_t
    y;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  transpose_image=CloneImage(image,image->rows,image->columns,MagickTrue,
    exception);
  if (transpose_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Transpose image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  transpose_view=AcquireAuthenticCacheView(transpose_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,transpose_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,(ssize_t) image->rows-y-1,
      image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(transpose_view,(ssize_t) (image->rows-y-1),
      0,1,transpose_image->rows,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
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
        PixelTrait transpose_traits=GetPixelChannelTraits(transpose_image,
          channel);
        if ((traits == UndefinedPixelTrait) ||
            (transpose_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(transpose_image,channel,p[i],q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(transpose_image);
    }
    if (SyncCacheViewAuthenticPixels(transpose_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,TransposeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  transpose_view=DestroyCacheView(transpose_view);
  image_view=DestroyCacheView(image_view);
  transpose_image->type=image->type;
  page=transpose_image->page;
  Swap(page.width,page.height);
  Swap(page.x,page.y);
  transpose_image->page=page;
  if (status == MagickFalse)
    transpose_image=DestroyImage(transpose_image);
  return(transpose_image);
}