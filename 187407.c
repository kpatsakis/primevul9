MagickExport MagickBooleanType PosterizeImage(Image *image,const size_t levels,
  const MagickBooleanType dither)
{
  MagickBooleanType
    status;

  status=PosterizeImageChannel(image,DefaultChannels,levels,dither);
  return(status);
}