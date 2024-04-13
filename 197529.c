MagickExport MagickBooleanType RandomThresholdImage(Image *image,
  const char *thresholds,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=RandomThresholdImageChannel(image,DefaultChannels,thresholds,
    exception);
  return(status);
}