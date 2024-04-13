MagickExport MagickBooleanType ContrastImage(Image *image,
  const MagickBooleanType sharpen,ExceptionInfo *exception)
{
#define ContrastImageTag  "Contrast/Image"

  CacheView
    *image_view;

  int
    sign;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  register ssize_t
    i;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  if (AccelerateContrastImage(image,sharpen,exception) != MagickFalse)
    return(MagickTrue);
#endif
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  sign=sharpen != MagickFalse ? 1 : -1;
  if (image->storage_class == PseudoClass)
    {
      /*
        Contrast enhance colormap.
      */
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        double
          blue,
          green,
          red;

        red=(double) image->colormap[i].red;
        green=(double) image->colormap[i].green;
        blue=(double) image->colormap[i].blue;
        Contrast(sign,&red,&green,&blue);
        image->colormap[i].red=(MagickRealType) red;
        image->colormap[i].green=(MagickRealType) green;
        image->colormap[i].blue=(MagickRealType) blue;
      }
    }
  /*
    Contrast enhance image.
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
    double
      blue,
      green,
      red;

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
      red=(double) GetPixelRed(image,q);
      green=(double) GetPixelGreen(image,q);
      blue=(double) GetPixelBlue(image,q);
      Contrast(sign,&red,&green,&blue);
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
        #pragma omp critical (MagickCore_ContrastImage)
#endif
        proceed=SetImageProgress(image,ContrastImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}