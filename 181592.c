MagickExport Image *TransverseImage(const Image *image,ExceptionInfo *exception)
{
#define TransverseImageTag  "Transverse/Image"

  CacheView
    *image_view,
    *transverse_view;

  Image
    *transverse_image;

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
  transverse_image=CloneImage(image,image->rows,image->columns,MagickTrue,
    exception);
  if (transverse_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Transverse image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  transverse_view=AcquireAuthenticCacheView(transverse_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,transverse_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickBooleanType
      sync;

    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(transverse_view,(ssize_t) (image->rows-y-1),
      0,1,transverse_image->rows,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    q+=GetPixelChannels(transverse_image)*image->columns;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      q-=GetPixelChannels(transverse_image);
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait transverse_traits=GetPixelChannelTraits(transverse_image,
          channel);
        if ((traits == UndefinedPixelTrait) ||
            (transverse_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(transverse_image,channel,p[i],q);
      }
      p+=GetPixelChannels(image);
    }
    sync=SyncCacheViewAuthenticPixels(transverse_view,exception);
    if (sync == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,TransverseImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  transverse_view=DestroyCacheView(transverse_view);
  image_view=DestroyCacheView(image_view);
  transverse_image->type=image->type;
  page=transverse_image->page;
  Swap(page.width,page.height);
  Swap(page.x,page.y);
  if (page.width != 0)
    page.x=(ssize_t) (page.width-transverse_image->columns-page.x);
  if (page.height != 0)
    page.y=(ssize_t) (page.height-transverse_image->rows-page.y);
  transverse_image->page=page;
  if (status == MagickFalse)
    transverse_image=DestroyImage(transverse_image);
  return(transverse_image);
}