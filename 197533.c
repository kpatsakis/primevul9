MagickExport MagickBooleanType BlackThresholdImage(Image *image,
  const char *threshold)
{
  MagickBooleanType
    status;

  status=BlackThresholdImageChannel(image,DefaultChannels,threshold,
    &image->exception);
  return(status);
}