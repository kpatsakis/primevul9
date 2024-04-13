MagickExport Image *ExtentImage(const Image *image,
  const RectangleInfo *geometry,ExceptionInfo *exception)
{
  Image
    *extent_image;

  MagickBooleanType
    status;

  /*
    Allocate extent image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  extent_image=CloneImage(image,geometry->width,geometry->height,MagickTrue,
    exception);
  if (extent_image == (Image *) NULL)
    return((Image *) NULL);
  status=SetImageBackgroundColor(extent_image,exception);
  if (status == MagickFalse)
    {
      extent_image=DestroyImage(extent_image);
      return((Image *) NULL);
    }
  status=CompositeImage(extent_image,image,image->compose,MagickTrue,
    -geometry->x,-geometry->y,exception);
  return(extent_image);
}