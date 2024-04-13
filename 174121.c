MagickExport MagickBooleanType GrayscaleImage(Image *image,
  const PixelIntensityMethod method,ExceptionInfo *exception)
{
#define GrayscaleImageTag  "Grayscale/Image"

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
      if (SyncImage(image,exception) == MagickFalse)
        return(MagickFalse);
      if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
        return(MagickFalse);
    }
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  if (AccelerateGrayscaleImage(image,method,exception) != MagickFalse)
    {
      image->intensity=method;
      image->type=GrayscaleType;
      return(SetImageColorspace(image,GRAYColorspace,exception));
    }
#endif
  /*
    Grayscale image.
  */
  status=MagickTrue;
  progress=0;
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
      MagickRealType
        blue,
        green,
        red,
        intensity;

      if (GetPixelWriteMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      red=(MagickRealType) GetPixelRed(image,q);
      green=(MagickRealType) GetPixelGreen(image,q);
      blue=(MagickRealType) GetPixelBlue(image,q);
      intensity=0.0;
      switch (method)
      {
        case AveragePixelIntensityMethod:
        {
          intensity=(red+green+blue)/3.0;
          break;
        }
        case BrightnessPixelIntensityMethod:
        {
          intensity=MagickMax(MagickMax(red,green),blue);
          break;
        }
        case LightnessPixelIntensityMethod:
        {
          intensity=(MagickMin(MagickMin(red,green),blue)+
            MagickMax(MagickMax(red,green),blue))/2.0;
          break;
        }
        case MSPixelIntensityMethod:
        {
          intensity=(MagickRealType) (((double) red*red+green*green+
            blue*blue)/3.0);
          break;
        }
        case Rec601LumaPixelIntensityMethod:
        {
          if (image->colorspace == RGBColorspace)
            {
              red=EncodePixelGamma(red);
              green=EncodePixelGamma(green);
              blue=EncodePixelGamma(blue);
            }
          intensity=0.298839*red+0.586811*green+0.114350*blue;
          break;
        }
        case Rec601LuminancePixelIntensityMethod:
        {
          if (image->colorspace == sRGBColorspace)
            {
              red=DecodePixelGamma(red);
              green=DecodePixelGamma(green);
              blue=DecodePixelGamma(blue);
            }
          intensity=0.298839*red+0.586811*green+0.114350*blue;
          break;
        }
        case Rec709LumaPixelIntensityMethod:
        default:
        {
          if (image->colorspace == RGBColorspace)
            {
              red=EncodePixelGamma(red);
              green=EncodePixelGamma(green);
              blue=EncodePixelGamma(blue);
            }
          intensity=0.212656*red+0.715158*green+0.072186*blue;
          break;
        }
        case Rec709LuminancePixelIntensityMethod:
        {
          if (image->colorspace == sRGBColorspace)
            {
              red=DecodePixelGamma(red);
              green=DecodePixelGamma(green);
              blue=DecodePixelGamma(blue);
            }
          intensity=0.212656*red+0.715158*green+0.072186*blue;
          break;
        }
        case RMSPixelIntensityMethod:
        {
          intensity=(MagickRealType) (sqrt((double) red*red+green*green+
            blue*blue)/sqrt(3.0));
          break;
        }
      }
      SetPixelGray(image,ClampToQuantum(intensity),q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_GrayscaleImage)
#endif
        proceed=SetImageProgress(image,GrayscaleImageTag,progress++,
           image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  image->intensity=method;
  image->type=GrayscaleType;
  return(SetImageColorspace(image,GRAYColorspace,exception));
}