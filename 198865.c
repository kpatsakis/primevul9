MagickExport MagickBooleanType WhiteThresholdImage(Image *image,
  const char *thresholds,ExceptionInfo *exception)
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

  PixelInfo
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
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  if (IsGrayColorspace(image->colorspace) != MagickFalse)
    (void) TransformImageColorspace(image,sRGBColorspace,exception);
  GetPixelInfo(image,&threshold);
  flags=ParseGeometry(thresholds,&geometry_info);
  threshold.red=geometry_info.rho;
  threshold.green=geometry_info.rho;
  threshold.blue=geometry_info.rho;
  threshold.black=geometry_info.rho;
  threshold.alpha=100.0;
  if ((flags & SigmaValue) != 0)
    threshold.green=geometry_info.sigma;
  if ((flags & XiValue) != 0)
    threshold.blue=geometry_info.xi;
  if ((flags & PsiValue) != 0)
    threshold.alpha=geometry_info.psi;
  if (threshold.colorspace == CMYKColorspace)
    {
      if ((flags & PsiValue) != 0)
        threshold.black=geometry_info.psi;
      if ((flags & ChiValue) != 0)
        threshold.alpha=geometry_info.chi;
    }
  if ((flags & PercentValue) != 0)
    {
      threshold.red*=(MagickRealType) (QuantumRange/100.0);
      threshold.green*=(MagickRealType) (QuantumRange/100.0);
      threshold.blue*=(MagickRealType) (QuantumRange/100.0);
      threshold.black*=(MagickRealType) (QuantumRange/100.0);
      threshold.alpha*=(MagickRealType) (QuantumRange/100.0);
    }
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
      double
        pixel;

      register ssize_t
        i;

      pixel=GetPixelIntensity(image,q);
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        if (image->channel_mask != DefaultChannels)
          pixel=(double) q[i];
        if (pixel > GetPixelInfoChannel(&threshold,channel))
          q[i]=QuantumRange;
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
        proceed=SetImageProgress(image,ThresholdImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}