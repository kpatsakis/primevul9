MagickExport MagickBooleanType WhiteThresholdImage(Image *image,
  const char *threshold)
{
  MagickBooleanType
    status;

  status=WhiteThresholdImageChannel(image,DefaultChannels,threshold,
    &image->exception);
  return(status);
}