MagickExport ColorspaceType GetImageColorspaceType(const Image *image,
  ExceptionInfo *exception)
{
  ColorspaceType
    colorspace;

  ImageType
    type;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  colorspace=image->colorspace;
  type=IdentifyImageType(image,exception);
  if ((type == BilevelType) || (type == GrayscaleType) ||
      (type == GrayscaleAlphaType))
    colorspace=GRAYColorspace;
  return(colorspace);
}