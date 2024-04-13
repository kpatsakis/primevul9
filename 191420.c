MagickExport MagickBooleanType SetImageGray(Image *image,
  ExceptionInfo *exception)
{
  const char
    *value;

  ImageType
    type;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (IsImageGray(image) != MagickFalse)
    return(MagickTrue);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    return(MagickFalse);
  value=GetImageProperty(image,"colorspace:auto-grayscale",exception);
  if (IsStringFalse(value) != MagickFalse)
    return(MagickFalse);
  type=IdentifyImageGray(image,exception);
  if (type == UndefinedType)
    return(MagickFalse);
  image->colorspace=GRAYColorspace;
  if (SyncImagePixelCache((Image *) image,exception) == MagickFalse)
    return(MagickFalse);
  image->type=type;
  return(MagickTrue);
}