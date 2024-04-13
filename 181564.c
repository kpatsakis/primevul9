MagickExport MagickBooleanType WriteStream(const ImageInfo *image_info,
  Image *image,StreamHandler stream,ExceptionInfo *exception)
{
  ImageInfo
    *write_info;

  MagickBooleanType
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  write_info=CloneImageInfo(image_info);
  *write_info->magick='\0';
  write_info->stream=stream;
  status=WriteImage(write_info,image,exception);
  write_info=DestroyImageInfo(write_info);
  return(status);
}