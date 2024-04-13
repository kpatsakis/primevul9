MagickExport MagickBooleanType HaldClutImage(Image *image,
  const Image *hald_image,ExceptionInfo *exception)
{
#define HaldClutImageTag  "Clut/Image"

  typedef struct _HaldInfo
  {
    double
      x,
      y,
      z;
  } HaldInfo;

  CacheView
    *hald_view,
    *image_view;

  double
    width;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelInfo
    zero;

  size_t
    cube_size,
    length,
    level;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(hald_image != (Image *) NULL);
  assert(hald_image->signature == MagickCoreSignature);
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  if (image->alpha_trait == UndefinedPixelTrait)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel,exception);
  /*
    Hald clut image.
  */
  status=MagickTrue;
  progress=0;
  length=(size_t) MagickMin((MagickRealType) hald_image->columns,
    (MagickRealType) hald_image->rows);
  for (level=2; (level*level*level) < length; level++) ;
  level*=level;
  cube_size=level*level;
  width=(double) hald_image->columns;
  GetPixelInfo(hald_image,&zero);
  hald_view=AcquireVirtualCacheView(hald_image,exception);
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
      double
        offset;

      HaldInfo
        point;

      PixelInfo
        pixel,
        pixel1,
        pixel2,
        pixel3,
        pixel4;

      point.x=QuantumScale*(level-1.0)*GetPixelRed(image,q);
      point.y=QuantumScale*(level-1.0)*GetPixelGreen(image,q);
      point.z=QuantumScale*(level-1.0)*GetPixelBlue(image,q);
      offset=point.x+level*floor(point.y)+cube_size*floor(point.z);
      point.x-=floor(point.x);
      point.y-=floor(point.y);
      point.z-=floor(point.z);
      pixel1=zero;
      (void) InterpolatePixelInfo(hald_image,hald_view,hald_image->interpolate,
        fmod(offset,width),floor(offset/width),&pixel1,exception);
      pixel2=zero;
      (void) InterpolatePixelInfo(hald_image,hald_view,hald_image->interpolate,
        fmod(offset+level,width),floor((offset+level)/width),&pixel2,exception);
      pixel3=zero;
      CompositePixelInfoAreaBlend(&pixel1,pixel1.alpha,&pixel2,pixel2.alpha,
        point.y,&pixel3);
      offset+=cube_size;
      (void) InterpolatePixelInfo(hald_image,hald_view,hald_image->interpolate,
        fmod(offset,width),floor(offset/width),&pixel1,exception);
      (void) InterpolatePixelInfo(hald_image,hald_view,hald_image->interpolate,
        fmod(offset+level,width),floor((offset+level)/width),&pixel2,exception);
      pixel4=zero;
      CompositePixelInfoAreaBlend(&pixel1,pixel1.alpha,&pixel2,pixel2.alpha,
        point.y,&pixel4);
      pixel=zero;
      CompositePixelInfoAreaBlend(&pixel3,pixel3.alpha,&pixel4,pixel4.alpha,
        point.z,&pixel);
      if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
        SetPixelRed(image,ClampToQuantum(pixel.red),q);
      if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
        SetPixelGreen(image,ClampToQuantum(pixel.green),q);
      if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
        SetPixelBlue(image,ClampToQuantum(pixel.blue),q);
      if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
          (image->colorspace == CMYKColorspace))
        SetPixelBlack(image,ClampToQuantum(pixel.black),q);
      if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
          (image->alpha_trait != UndefinedPixelTrait))
        SetPixelAlpha(image,ClampToQuantum(pixel.alpha),q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_HaldClutImage)
#endif
        proceed=SetImageProgress(image,HaldClutImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  hald_view=DestroyCacheView(hald_view);
  image_view=DestroyCacheView(image_view);
  return(status);
}