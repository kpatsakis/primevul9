MagickExport Image *CropImage(const Image *image,const RectangleInfo *geometry,
  ExceptionInfo *exception)
{
#define CropImageTag  "Crop/Image"

  CacheView
    *crop_view,
    *image_view;

  Image
    *crop_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  OffsetInfo
    offset;

  RectangleInfo
    bounding_box,
    page;

  ssize_t
    y;

  /*
    Check crop geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  bounding_box=image->page;
  if ((bounding_box.width == 0) || (bounding_box.height == 0))
    {
      bounding_box.width=image->columns;
      bounding_box.height=image->rows;
    }
  page=(*geometry);
  if (page.width == 0)
    page.width=bounding_box.width;
  if (page.height == 0)
    page.height=bounding_box.height;
  if (((bounding_box.x-page.x) >= (ssize_t) page.width) ||
      ((bounding_box.y-page.y) >= (ssize_t) page.height) ||
      ((page.x-bounding_box.x) > (ssize_t) image->columns) ||
      ((page.y-bounding_box.y) > (ssize_t) image->rows))
    {
      /*
        Crop is not within virtual canvas, return 1 pixel transparent image.
      */
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "GeometryDoesNotContainImage","`%s'",image->filename);
      crop_image=CloneImage(image,1,1,MagickTrue,exception);
      if (crop_image == (Image *) NULL)
        return((Image *) NULL);
      crop_image->background_color.alpha=(MagickRealType) TransparentAlpha;
      crop_image->alpha_trait=BlendPixelTrait;
      (void) SetImageBackgroundColor(crop_image,exception);
      crop_image->page=bounding_box;
      crop_image->page.x=(-1);
      crop_image->page.y=(-1);
      if (crop_image->dispose == BackgroundDispose)
        crop_image->dispose=NoneDispose;
      return(crop_image);
    }
  if ((page.x < 0) && (bounding_box.x >= 0))
    {
      page.width+=page.x-bounding_box.x;
      page.x=0;
    }
  else
    {
      page.width-=bounding_box.x-page.x;
      page.x-=bounding_box.x;
      if (page.x < 0)
        page.x=0;
    }
  if ((page.y < 0) && (bounding_box.y >= 0))
    {
      page.height+=page.y-bounding_box.y;
      page.y=0;
    }
  else
    {
      page.height-=bounding_box.y-page.y;
      page.y-=bounding_box.y;
      if (page.y < 0)
        page.y=0;
    }
  if ((page.x+(ssize_t) page.width) > (ssize_t) image->columns)
    page.width=image->columns-page.x;
  if ((geometry->width != 0) && (page.width > geometry->width))
    page.width=geometry->width;
  if ((page.y+(ssize_t) page.height) > (ssize_t) image->rows)
    page.height=image->rows-page.y;
  if ((geometry->height != 0) && (page.height > geometry->height))
    page.height=geometry->height;
  bounding_box.x+=page.x;
  bounding_box.y+=page.y;
  if ((page.width == 0) || (page.height == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "GeometryDoesNotContainImage","`%s'",image->filename);
      return((Image *) NULL);
    }
  /*
    Initialize crop image attributes.
  */
  crop_image=CloneImage(image,page.width,page.height,MagickTrue,exception);
  if (crop_image == (Image *) NULL)
    return((Image *) NULL);
  crop_image->page.width=image->page.width;
  crop_image->page.height=image->page.height;
  offset.x=(ssize_t) (bounding_box.x+bounding_box.width);
  offset.y=(ssize_t) (bounding_box.y+bounding_box.height);
  if ((offset.x > (ssize_t) image->page.width) ||
      (offset.y > (ssize_t) image->page.height))
    {
      crop_image->page.width=bounding_box.width;
      crop_image->page.height=bounding_box.height;
    }
  crop_image->page.x=bounding_box.x;
  crop_image->page.y=bounding_box.y;
  /*
    Crop image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  crop_view=AcquireAuthenticCacheView(crop_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,crop_image,crop_image->rows,1)
#endif
  for (y=0; y < (ssize_t) crop_image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,page.x,page.y+y,crop_image->columns,
      1,exception);
    q=QueueCacheViewAuthenticPixels(crop_view,0,y,crop_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) crop_image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait crop_traits=GetPixelChannelTraits(crop_image,channel);
        if ((traits == UndefinedPixelTrait) ||
            (crop_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(crop_image,channel,p[i],q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(crop_image);
    }
    if (SyncCacheViewAuthenticPixels(crop_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,CropImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  crop_view=DestroyCacheView(crop_view);
  image_view=DestroyCacheView(image_view);
  crop_image->type=image->type;
  if (status == MagickFalse)
    crop_image=DestroyImage(crop_image);
  return(crop_image);
}