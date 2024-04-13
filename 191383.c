MagickExport MagickBooleanType TransformImageColorspace(Image *image,
  const ColorspaceType colorspace,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->colorspace == colorspace)
    return(SetImageColorspace(image,colorspace,exception));
  (void) DeleteImageProfile(image,"icc");
  (void) DeleteImageProfile(image,"icm");
  if (colorspace == UndefinedColorspace)
    return(SetImageColorspace(image,colorspace,exception));
  /*
    Convert the reference image from an alternate colorspace to sRGB.
  */
  if (IssRGBColorspace(colorspace) != MagickFalse)
    return(TransformsRGBImage(image,exception));
  status=MagickTrue;
  if (IssRGBColorspace(image->colorspace) == MagickFalse)
    status=TransformsRGBImage(image,exception);
  if (status == MagickFalse)
    return(status);
  /*
    Convert the reference image from sRGB to an alternate colorspace.
  */
  if (sRGBTransformImage(image,colorspace,exception) == MagickFalse)
    status=MagickFalse;
  return(status);
}