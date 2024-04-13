MagickExport MagickBooleanType WhiteThresholdImageChannel(Image *image,
  const ChannelType channel,const char *thresholds,ExceptionInfo *exception)
{
#define ThresholdImageTag  "Threshold/Image"

  CacheView
    *image_view;

  GeometryInfo
    geometry_info;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    threshold;

  MagickStatusType
    flags;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (thresholds == (const char *) NULL)
    return(MagickTrue);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  flags=ParseGeometry(thresholds,&geometry_info);
  GetMagickPixelPacket(image,&threshold);
  threshold.red=geometry_info.rho;
  threshold.green=geometry_info.sigma;
  if ((flags & SigmaValue) == 0)
    threshold.green=threshold.red;
  threshold.blue=geometry_info.xi;
  if ((flags & XiValue) == 0)
    threshold.blue=threshold.red;
  threshold.opacity=geometry_info.psi;
  if ((flags & PsiValue) == 0)
    threshold.opacity=threshold.red;
  threshold.index=geometry_info.chi;
  if ((flags & ChiValue) == 0)
    threshold.index=threshold.red;
  if ((flags & PercentValue) != 0)
    {
      threshold.red*=(MagickRealType) (QuantumRange/100.0);
      threshold.green*=(MagickRealType) (QuantumRange/100.0);
      threshold.blue*=(MagickRealType) (QuantumRange/100.0);
      threshold.opacity*=(MagickRealType) (QuantumRange/100.0);
      threshold.index*=(MagickRealType) (QuantumRange/100.0);
    }
  if ((IsMagickGray(&threshold) == MagickFalse) &&
      (IsGrayColorspace(image->colorspace) != MagickFalse))
    (void) SetImageColorspace(image,sRGBColorspace);
  /*
    White threshold image.
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
    register IndexPacket
      *magick_restrict indexes;

    register ssize_t
      x;

    register PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (((channel & RedChannel) != 0) &&
          ((MagickRealType) GetPixelRed(q) > threshold.red))
        SetPixelRed(q,QuantumRange);
      if (((channel & GreenChannel) != 0) &&
          ((MagickRealType) GetPixelGreen(q) > threshold.green))
        SetPixelGreen(q,QuantumRange);
      if (((channel & BlueChannel) != 0) &&
          ((MagickRealType) GetPixelBlue(q) > threshold.blue))
        SetPixelBlue(q,QuantumRange);
      if (((channel & OpacityChannel) != 0) &&
          ((MagickRealType) GetPixelOpacity(q) > threshold.opacity))
        SetPixelOpacity(q,QuantumRange);
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace) &&
          ((MagickRealType) GetPixelIndex(indexes+x)) > threshold.index)
        SetPixelIndex(indexes+x,QuantumRange);
      q++;
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
        proceed=SetImageProgress(image,ThresholdImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}