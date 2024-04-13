MagickExport MagickBooleanType SetImageMonochrome(Image *image,
  ExceptionInfo *exception)
{
  const char
    *value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->type == BilevelType)
    return(MagickTrue);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    return(MagickFalse);
  value=GetImageProperty(image,"colorspace:auto-grayscale",exception);
  if (IsStringFalse(value) != MagickFalse)
    return(MagickFalse);
  if (IdentifyImageMonochrome(image,exception) == MagickFalse)
    return(MagickFalse);
  image->colorspace=GRAYColorspace;
  if (SyncImagePixelCache((Image *) image,exception) == MagickFalse)
    return(MagickFalse);
  image->type=BilevelType;
  return(MagickTrue);
}