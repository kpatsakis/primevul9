MagickPrivate MagickBooleanType XRenderImage(Image *image,
  const DrawInfo *draw_info,const PointInfo *offset,TypeMetric *metrics,
  ExceptionInfo *exception)
{
  (void) draw_info;
  (void) offset;
  (void) metrics;
  (void) ThrowMagickException(exception,GetMagickModule(),
    MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","'%s' (X11)",
    image->filename);
  return(MagickFalse);
}