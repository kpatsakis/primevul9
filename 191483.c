MagickExport MagickBooleanType SetImageColorspace(Image *image,
  const ColorspaceType colorspace,ExceptionInfo *exception)
{
  ImageType
    type;

  MagickBooleanType
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (image->colorspace == colorspace)
    return(MagickTrue);
  image->colorspace=colorspace;
  image->rendering_intent=UndefinedIntent;
  image->gamma=1.000/2.200;
  (void) memset(&image->chromaticity,0,sizeof(image->chromaticity));
  type=image->type;
  if (IsGrayColorspace(colorspace) != MagickFalse)
    {
      if (colorspace == LinearGRAYColorspace)
        image->gamma=1.000;
      type=GrayscaleType;
    }
  else
    if ((IsRGBColorspace(colorspace) != MagickFalse) ||
        (colorspace == XYZColorspace) || (colorspace == xyYColorspace))
      image->gamma=1.000;
    else
      {
        image->rendering_intent=PerceptualIntent;
        image->chromaticity.red_primary.x=0.6400;
        image->chromaticity.red_primary.y=0.3300;
        image->chromaticity.red_primary.z=0.0300;
        image->chromaticity.green_primary.x=0.3000;
        image->chromaticity.green_primary.y=0.6000;
        image->chromaticity.green_primary.z=0.1000;
        image->chromaticity.blue_primary.x=0.1500;
        image->chromaticity.blue_primary.y=0.0600;
        image->chromaticity.blue_primary.z=0.7900;
        image->chromaticity.white_point.x=0.3127;
        image->chromaticity.white_point.y=0.3290;
        image->chromaticity.white_point.z=0.3583;
      }
  status=SyncImagePixelCache(image,exception);
  image->type=type;
  return(status);
}