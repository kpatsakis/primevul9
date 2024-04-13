static Image *ReadMNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    logging,
    status;

  MngInfo
    *mng_info;

  /* Open image file.  */

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"Enter ReadMNGImage()");
  image=AcquireImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);

  if (status == MagickFalse)
    return(DestroyImageList(image));

  /* Allocate a MngInfo structure.  */

  mng_info=(MngInfo *) AcquireMagickMemory(sizeof(MngInfo));

  if (mng_info == (MngInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

  /* Initialize members of the MngInfo structure.  */

  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  image=ReadOneMNGImage(mng_info,image_info,exception);
  mng_info=MngInfoFreeStruct(mng_info);

  if (image == (Image *) NULL)
    {
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "exit ReadMNGImage() with error");

      return((Image *) NULL);
    }
  (void) CloseBlob(image);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit ReadMNGImage()");

  return(GetFirstImageInList(image));
}