MagickExport MagickBooleanType BilevelImage(Image *image,const double threshold)
{
  MagickBooleanType
    status;

  status=BilevelImageChannel(image,DefaultChannels,threshold);
  return(status);
}