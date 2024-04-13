static MagickBooleanType WriteMETAImage(const ImageInfo *image_info,
  Image *image)
{
  const StringInfo
    *profile;

  MagickBooleanType
    status;

  size_t
    length;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=0;
  if (LocaleCompare(image_info->magick,"8BIM") == 0)
    {
      /*
        Write 8BIM image.
      */
      profile=GetImageProfile(image,"8bim");
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == MagickFalse)
        return(status);
      (void) WriteBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  if (LocaleCompare(image_info->magick,"iptc") == 0)
    {
      size_t
        length;

      unsigned char
        *info;

      profile=GetImageProfile(image,"iptc");
      if (profile == (StringInfo *) NULL)
        profile=GetImageProfile(image,"8bim");
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      info=GetStringInfoDatum(profile);
      length=GetStringInfoLength(profile);
      length=GetIPTCStream(&info,length);
      if (length == 0)
        ThrowWriterException(CoderError,"NoIPTCProfileAvailable");
      (void) WriteBlob(image,length,info);
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  if (LocaleCompare(image_info->magick,"8BIMTEXT") == 0)
    {
      Image
        *buff;

      profile=GetImageProfile(image,"8bim");
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == MagickFalse)
        return(status);
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      AttachBlob(buff->blob,GetStringInfoDatum(profile),
        GetStringInfoLength(profile));
      format8BIM(buff,image);
      (void) DetachBlob(buff->blob);
      buff=DestroyImage(buff);
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  if (LocaleCompare(image_info->magick,"8BIMWTEXT") == 0)
    return(MagickFalse);
  if (LocaleCompare(image_info->magick,"IPTCTEXT") == 0)
    {
      Image
        *buff;

      unsigned char
        *info;

      profile=GetImageProfile(image,"8bim");
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable");
      info=GetStringInfoDatum(profile);
      length=GetStringInfoLength(profile);
      length=GetIPTCStream(&info,length);
      if (length == 0)
        ThrowWriterException(CoderError,"NoIPTCProfileAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == MagickFalse)
        return(status);
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      AttachBlob(buff->blob,info,length);
      formatIPTC(buff,image);
      (void) DetachBlob(buff->blob);
      buff=DestroyImage(buff);
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  if (LocaleCompare(image_info->magick,"IPTCWTEXT") == 0)
    return(MagickFalse);
  if ((LocaleCompare(image_info->magick,"APP1") == 0) ||
      (LocaleCompare(image_info->magick,"EXIF") == 0) ||
      (LocaleCompare(image_info->magick,"XMP") == 0))
    {
      /*
        (void) Write APP1 image.
      */
      profile=GetImageProfile(image,image_info->magick);
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"NoAPP1DataIsAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == MagickFalse)
        return(status);
      (void) WriteBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  if ((LocaleCompare(image_info->magick,"ICC") == 0) ||
      (LocaleCompare(image_info->magick,"ICM") == 0))
    {
      /*
        Write ICM image.
      */
      profile=GetImageProfile(image,"icc");
      if (profile == (StringInfo *) NULL)
        ThrowWriterException(CoderError,"NoColorProfileIsAvailable");
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == MagickFalse)
        return(status);
      (void) WriteBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  return(MagickFalse);
}