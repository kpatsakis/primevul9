MagickExport Image *XImportImage(const ImageInfo *image_info,
  XImportInfo *ximage_info,ExceptionInfo *exception)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(ximage_info != (XImportInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  (void) exception;
  return((Image *) NULL);
}