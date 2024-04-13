MagickExport MagickBooleanType LinearStretchImage(Image *image,
  const double black_point,const double white_point,ExceptionInfo *exception)
{
#define LinearStretchImageTag  "LinearStretch/Image"

  CacheView
    *image_view;

  double
    *histogram,
    intensity;

  MagickBooleanType
    status;

  ssize_t
    black,
    white,
    y;

  /*
    Allocate histogram and linear map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  histogram=(double *) AcquireQuantumMemory(MaxMap+1UL,sizeof(*histogram));
  if (histogram == (double *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  /*
    Form histogram.
  */
  (void) ResetMagickMemory(histogram,0,(MaxMap+1)*sizeof(*histogram));
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      intensity=GetPixelIntensity(image,p);
      histogram[ScaleQuantumToMap(ClampToQuantum(intensity))]++;
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  /*
    Find the histogram boundaries by locating the black and white point levels.
  */
  intensity=0.0;
  for (black=0; black < (ssize_t) MaxMap; black++)
  {
    intensity+=histogram[black];
    if (intensity >= black_point)
      break;
  }
  intensity=0.0;
  for (white=(ssize_t) MaxMap; white != 0; white--)
  {
    intensity+=histogram[white];
    if (intensity >= white_point)
      break;
  }
  histogram=(double *) RelinquishMagickMemory(histogram);
  status=LevelImage(image,(double) ScaleMapToQuantum((MagickRealType) black),
    (double) ScaleMapToQuantum((MagickRealType) white),1.0,exception);
  return(status);
}