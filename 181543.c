MagickExport Image *PingImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *ping_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  ping_info=CloneImageInfo(image_info);
  ping_info->ping=MagickTrue;
  image=ReadStream(ping_info,&PingStream,exception);
  if (image != (Image *) NULL)
    {
      ResetTimer(&image->timer);
      if (ping_info->verbose != MagickFalse)
        (void) IdentifyImage(image,stdout,MagickFalse,exception);
    }
  ping_info=DestroyImageInfo(ping_info);
  return(image);
}