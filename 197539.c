MagickExport MagickBooleanType OrderedDitherImage(Image *image)
{
  MagickBooleanType
    status;

  status=OrderedDitherImageChannel(image,DefaultChannels,&image->exception);
  return(status);
}