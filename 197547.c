MagickExport MagickBooleanType PerceptibleImage(Image *image,
  const double epsilon)
{
  MagickBooleanType
    status;

  status=PerceptibleImageChannel(image,DefaultChannels,epsilon);
  return(status);
}