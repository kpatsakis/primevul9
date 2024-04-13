MagickExport Image *ChopImage(const Image *image,const RectangleInfo *chop_info,
  ExceptionInfo *exception)
{
#define ChopImageTag  "Chop/Image"

  CacheView
    *chop_view,
    *image_view;

  Image
    *chop_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  RectangleInfo
    extent;

  ssize_t
    y;

  /*
    Check chop geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  assert(chop_info != (RectangleInfo *) NULL);
  if (((chop_info->x+(ssize_t) chop_info->width) < 0) ||
      ((chop_info->y+(ssize_t) chop_info->height) < 0) ||
      (chop_info->x > (ssize_t) image->columns) ||
      (chop_info->y > (ssize_t) image->rows))
    ThrowImageException(OptionWarning,"GeometryDoesNotContainImage");
  extent=(*chop_info);
  if ((extent.x+(ssize_t) extent.width) > (ssize_t) image->columns)
    extent.width=(size_t) ((ssize_t) image->columns-extent.x);
  if ((extent.y+(ssize_t) extent.height) > (ssize_t) image->rows)
    extent.height=(size_t) ((ssize_t) image->rows-extent.y);
  if (extent.x < 0)
    {
      extent.width-=(size_t) (-extent.x);
      extent.x=0;
    }
  if (extent.y < 0)
    {
      extent.height-=(size_t) (-extent.y);
      extent.y=0;
    }
  chop_image=CloneImage(image,image->columns-extent.width,image->rows-
    extent.height,MagickTrue,exception);
  if (chop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Extract chop image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  chop_view=AcquireAuthenticCacheView(chop_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,chop_image,extent.y,1)
#endif
  for (y=0; y < (ssize_t) extent.y; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(chop_view,0,y,chop_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((x < extent.x) || (x >= (ssize_t) (extent.x+extent.width)))
        {
          register ssize_t
            i;

          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(image,i);
            PixelTrait traits = GetPixelChannelTraits(image,channel);
            PixelTrait chop_traits=GetPixelChannelTraits(chop_image,channel);
            if ((traits == UndefinedPixelTrait) ||
                (chop_traits == UndefinedPixelTrait))
              continue;
            SetPixelChannel(chop_image,channel,p[i],q);
          }
          q+=GetPixelChannels(chop_image);
        }
      p+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(chop_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ChopImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  /*
    Extract chop image.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,chop_image,image->rows-(extent.y+extent.height),1)
#endif
  for (y=0; y < (ssize_t) (image->rows-(extent.y+extent.height)); y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,extent.y+extent.height+y,
      image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(chop_view,0,extent.y+y,chop_image->columns,
      1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((x < extent.x) || (x >= (ssize_t) (extent.x+extent.width)))
        {
          register ssize_t
            i;

          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(image,i);
            PixelTrait traits = GetPixelChannelTraits(image,channel);
            PixelTrait chop_traits=GetPixelChannelTraits(chop_image,channel);
            if ((traits == UndefinedPixelTrait) ||
                (chop_traits == UndefinedPixelTrait))
              continue;
            SetPixelChannel(chop_image,channel,p[i],q);
          }
          q+=GetPixelChannels(chop_image);
        }
      p+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(chop_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ChopImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  chop_view=DestroyCacheView(chop_view);
  image_view=DestroyCacheView(image_view);
  chop_image->type=image->type;
  if (status == MagickFalse)
    chop_image=DestroyImage(chop_image);
  return(chop_image);
}