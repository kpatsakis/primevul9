MagickExport MagickBooleanType ClampImage(Image *image)
{
  MagickBooleanType
    status;

  status=ClampImageChannel(image,DefaultChannels);
  return(status);
}