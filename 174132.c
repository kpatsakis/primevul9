MagickExport MagickBooleanType ClutImage(Image *image,const Image *clut_image,
  const PixelInterpolateMethod method,ExceptionInfo *exception)
{
#define ClutImageTag  "Clut/Image"

  CacheView
    *clut_view,
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelInfo
    *clut_map;

  register ssize_t
    i;

  ssize_t adjust,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(clut_image != (Image *) NULL);
  assert(clut_image->signature == MagickCoreSignature);
  if( SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  if( (IsGrayColorspace(image->colorspace) != MagickFalse) &&
      (IsGrayColorspace(clut_image->colorspace) == MagickFalse))
    (void) SetImageColorspace(image,sRGBColorspace,exception);
  clut_map=(PixelInfo *) AcquireQuantumMemory(MaxMap+1UL,sizeof(*clut_map));
  if (clut_map == (PixelInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  /*
    Clut image.
  */
  status=MagickTrue;
  progress=0;
  adjust=(ssize_t) (clut_image->interpolate == IntegerInterpolatePixel ? 0 : 1);
  clut_view=AcquireVirtualCacheView(clut_image,exception);
  for (i=0; i <= (ssize_t) MaxMap; i++)
  {
    GetPixelInfo(clut_image,clut_map+i);
    (void) InterpolatePixelInfo(clut_image,clut_view,method,
      (double) i*(clut_image->columns-adjust)/MaxMap,(double) i*
      (clut_image->rows-adjust)/MaxMap,clut_map+i,exception);
  }
  clut_view=DestroyCacheView(clut_view);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    PixelInfo
      pixel;

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
    GetPixelInfo(image,&pixel);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      PixelTrait
        traits;

      if (GetPixelWriteMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      GetPixelInfoPixel(image,q,&pixel);
      traits=GetPixelChannelTraits(image,RedPixelChannel);
      if ((traits & UpdatePixelTrait) != 0)
        pixel.red=clut_map[ScaleQuantumToMap(ClampToQuantum(
          pixel.red))].red;
      traits=GetPixelChannelTraits(image,GreenPixelChannel);
      if ((traits & UpdatePixelTrait) != 0)
        pixel.green=clut_map[ScaleQuantumToMap(ClampToQuantum(
          pixel.green))].green;
      traits=GetPixelChannelTraits(image,BluePixelChannel);
      if ((traits & UpdatePixelTrait) != 0)
        pixel.blue=clut_map[ScaleQuantumToMap(ClampToQuantum(
          pixel.blue))].blue;
      traits=GetPixelChannelTraits(image,BlackPixelChannel);
      if ((traits & UpdatePixelTrait) != 0)
        pixel.black=clut_map[ScaleQuantumToMap(ClampToQuantum(
          pixel.black))].black;
      traits=GetPixelChannelTraits(image,AlphaPixelChannel);
      if ((traits & UpdatePixelTrait) != 0)
        pixel.alpha=clut_map[ScaleQuantumToMap(ClampToQuantum(
          pixel.alpha))].alpha;
      SetPixelViaPixelInfo(image,&pixel,q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_ClutImage)
#endif
        proceed=SetImageProgress(image,ClutImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  clut_map=(PixelInfo *) RelinquishMagickMemory(clut_map);
  if ((clut_image->alpha_trait != UndefinedPixelTrait) &&
      ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0))
    (void) SetImageAlphaChannel(image,ActivateAlphaChannel,exception);
  return(status);
}