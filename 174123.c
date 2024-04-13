MagickExport MagickBooleanType ModulateImage(Image *image,const char *modulate,
  ExceptionInfo *exception)
{
#define ModulateImageTag  "Modulate/Image"

  CacheView
    *image_view;

  ColorspaceType
    colorspace;

  const char
    *artifact;

  double
    percent_brightness,
    percent_hue,
    percent_saturation;

  GeometryInfo
    geometry_info;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickStatusType
    flags;

  register ssize_t
    i;

  ssize_t
    y;

  /*
    Initialize modulate table.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (modulate == (char *) NULL)
    return(MagickFalse);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    (void) SetImageColorspace(image,sRGBColorspace,exception);
  flags=ParseGeometry(modulate,&geometry_info);
  percent_brightness=geometry_info.rho;
  percent_saturation=geometry_info.sigma;
  if ((flags & SigmaValue) == 0)
    percent_saturation=100.0;
  percent_hue=geometry_info.xi;
  if ((flags & XiValue) == 0)
    percent_hue=100.0;
  colorspace=UndefinedColorspace;
  artifact=GetImageArtifact(image,"modulate:colorspace");
  if (artifact != (const char *) NULL)
    colorspace=(ColorspaceType) ParseCommandOption(MagickColorspaceOptions,
      MagickFalse,artifact);
  if (image->storage_class == PseudoClass)
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      double
        blue,
        green,
        red;

      /*
        Modulate image colormap.
      */
      red=(double) image->colormap[i].red;
      green=(double) image->colormap[i].green;
      blue=(double) image->colormap[i].blue;
      switch (colorspace)
      {
        case HCLColorspace:
        {
          ModulateHCL(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HCLpColorspace:
        {
          ModulateHCLp(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSBColorspace:
        {
          ModulateHSB(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSIColorspace:
        {
          ModulateHSI(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSLColorspace:
        default:
        {
          ModulateHSL(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSVColorspace:
        {
          ModulateHSV(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HWBColorspace:
        {
          ModulateHWB(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case LCHColorspace:
        case LCHabColorspace:
        {
          ModulateLCHab(percent_brightness,percent_saturation,percent_hue,
            &red,&green,&blue);
          break;
        }
        case LCHuvColorspace:
        {
          ModulateLCHuv(percent_brightness,percent_saturation,percent_hue,
            &red,&green,&blue);
          break;
        }
      }
      image->colormap[i].red=red;
      image->colormap[i].green=green;
      image->colormap[i].blue=blue;
    }
  /*
    Modulate image.
  */
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  if (AccelerateModulateImage(image,percent_brightness,percent_hue,
        percent_saturation,colorspace,exception) != MagickFalse)
    return(MagickTrue);
#endif
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
      double
        blue,
        green,
        red;

      if (GetPixelWriteMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      red=(double) GetPixelRed(image,q);
      green=(double) GetPixelGreen(image,q);
      blue=(double) GetPixelBlue(image,q);
      switch (colorspace)
      {
        case HCLColorspace:
        {
          ModulateHCL(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HCLpColorspace:
        {
          ModulateHCLp(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSBColorspace:
        {
          ModulateHSB(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSLColorspace:
        default:
        {
          ModulateHSL(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HSVColorspace:
        {
          ModulateHSV(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case HWBColorspace:
        {
          ModulateHWB(percent_hue,percent_saturation,percent_brightness,
            &red,&green,&blue);
          break;
        }
        case LCHabColorspace:
        {
          ModulateLCHab(percent_brightness,percent_saturation,percent_hue,
            &red,&green,&blue);
          break;
        }
        case LCHColorspace:
        case LCHuvColorspace:
        {
          ModulateLCHuv(percent_brightness,percent_saturation,percent_hue,
            &red,&green,&blue);
          break;
        }
      }
      SetPixelRed(image,ClampToQuantum(red),q);
      SetPixelGreen(image,ClampToQuantum(green),q);
      SetPixelBlue(image,ClampToQuantum(blue),q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_ModulateImage)
#endif
        proceed=SetImageProgress(image,ModulateImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}