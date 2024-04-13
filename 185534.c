static MagickBooleanType WriteJNGImage(const ImageInfo *image_info,
  Image *image, ExceptionInfo *exception)
{
  MagickBooleanType
    logging,
    status;

  MngInfo
    *mng_info;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"Enter WriteJNGImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if ((image->columns > 65535UL) || (image->rows > 65535UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");

  /*
    Allocate a MngInfo structure.
  */
  mng_info=(MngInfo *) AcquireMagickMemory(sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Initialize members of the MngInfo structure.
  */
  (void) ResetMagickMemory(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;

  (void) WriteBlob(image,8,(const unsigned char *) "\213JNG\r\n\032\n");

  status=WriteOneJNGImage(mng_info,image_info,image,exception);
  mng_info=MngInfoFreeStruct(mng_info);
  (void) CloseBlob(image);

  (void) CatchImageException(image);
  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WriteJNGImage()");
  return(status);
}