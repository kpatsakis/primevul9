MagickExport MagickBooleanType OrderedPosterizeImage(Image *image,
  const char *threshold_map,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=OrderedPosterizeImageChannel(image,DefaultChannels,threshold_map,
    exception);
  return(status);
}