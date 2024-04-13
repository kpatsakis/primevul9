MagickExport MagickBooleanType AutoThresholdImage(Image *image,
  const AutoThresholdMethod method,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  char
    property[MagickPathExtent];

  double
    gamma,
    *histogram,
    sum,
    threshold;

  MagickBooleanType
    status;

  register ssize_t
    i;

  ssize_t
    y;

  /*
    Form histogram.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  histogram=(double *) AcquireQuantumMemory(MaxIntensity+1UL,
    sizeof(*histogram));
  if (histogram == (double *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  status=MagickTrue;
  (void) memset(histogram,0,(MaxIntensity+1UL)*sizeof(*histogram));
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
      double intensity = GetPixelIntensity(image,p);
      histogram[ScaleQuantumToChar(ClampToQuantum(intensity))]++;
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  /*
    Normalize histogram.
  */
  sum=0.0;
  for (i=0; i <= (ssize_t) MaxIntensity; i++)
    sum+=histogram[i];
  gamma=PerceptibleReciprocal(sum);
  for (i=0; i <= (ssize_t) MaxIntensity; i++)
    histogram[i]=gamma*histogram[i];
  /*
    Discover threshold from histogram.
  */
  switch (method)
  {
    case KapurThresholdMethod:
    {
      threshold=KapurThreshold(image,histogram,exception);
      break;
    }
    case OTSUThresholdMethod:
    default:
    {
      threshold=OTSUThreshold(image,histogram,exception);
      break;
    }
    case TriangleThresholdMethod:
    {
      threshold=TriangleThreshold(histogram);
      break;
    }
  }
  histogram=(double *) RelinquishMagickMemory(histogram);
  if (threshold < 0.0)
    status=MagickFalse;
  if (status == MagickFalse)
    return(MagickFalse);
  /*
    Threshold image.
  */
  (void) FormatLocaleString(property,MagickPathExtent,"%g%%",threshold);
  (void) SetImageProperty(image,"auto-threshold:threshold",property,exception);
  return(BilevelImage(image,QuantumRange*threshold/100.0,exception));
}