static Image *ReadMETAImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *buff,
    *image;

  MagickBooleanType
    status;

  StringInfo
    *profile;

  size_t
    length;

  void
    *blob;

  /*
    Open file containing binary metadata
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  image->columns=1;
  image->rows=1;
  if (SetImageBackgroundColor(image) == MagickFalse)
    {
      InheritException(exception,&image->exception);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  length=1;
  if (LocaleNCompare(image_info->magick,"8BIM",4) == 0)
    {
      /*
        Read 8BIM binary metadata.
      */
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      blob=(unsigned char *) AcquireQuantumMemory(length,sizeof(unsigned char));
      if (blob == (unsigned char *) NULL)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      AttachBlob(buff->blob,blob,length);
      if (LocaleCompare(image_info->magick,"8BIMTEXT") == 0)
        {
          length=(size_t) parse8BIM(image, buff);
          if (length & 1)
            (void) WriteBlobByte(buff,0x0);
        }
      else if (LocaleCompare(image_info->magick,"8BIMWTEXT") == 0)
        {
          length=(size_t) parse8BIMW(image, buff);
          if (length & 1)
            (void) WriteBlobByte(buff,0x0);
        }
      else
        CopyBlob(image,buff);
      profile=BlobToStringInfo(GetBlobStreamData(buff),(size_t)
        GetBlobSize(buff));
      if (profile == (StringInfo *) NULL)
        {
          blob=DetachBlob(buff->blob);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      status=SetImageProfile(image,"8bim",profile);
      profile=DestroyStringInfo(profile);
      blob=DetachBlob(buff->blob);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      buff=DestroyImage(buff);
      if (status == MagickFalse)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  if (LocaleNCompare(image_info->magick,"APP1",4) == 0)
    {
      char
        name[MaxTextExtent];

      (void) FormatLocaleString(name,MaxTextExtent,"APP%d",1);
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      blob=(unsigned char *) AcquireQuantumMemory(length,sizeof(unsigned char));
      if (blob == (unsigned char *) NULL)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      AttachBlob(buff->blob,blob,length);
      if (LocaleCompare(image_info->magick,"APP1JPEG") == 0)
        {
          Image
            *iptc;

          int
            result;

          if (image_info->profile == (void *) NULL)
            {
              blob=DetachBlob(buff->blob);
              blob=(unsigned char *) RelinquishMagickMemory(blob);
              buff=DestroyImage(buff);
              ThrowReaderException(CoderError,"NoIPTCProfileAvailable");
            }
          profile=CloneStringInfo((StringInfo *) image_info->profile);
          iptc=AcquireImage((ImageInfo *) NULL);
          if (iptc == (Image *) NULL)
            {
              blob=DetachBlob(buff->blob);
              blob=(unsigned char *) RelinquishMagickMemory(blob);
              buff=DestroyImage(buff);
              ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
            }
          AttachBlob(iptc->blob,GetStringInfoDatum(profile),
            GetStringInfoLength(profile));
          result=jpeg_embed(image,buff,iptc);
          blob=DetachBlob(iptc->blob);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          iptc=DestroyImage(iptc);
          if (result == 0)
            ThrowReaderException(CoderError,"JPEGEmbeddingFailed");
        }
      else
        CopyBlob(image,buff);
      profile=BlobToStringInfo(GetBlobStreamData(buff),(size_t)
        GetBlobSize(buff));
      if (profile == (StringInfo *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      status=SetImageProfile(image,name,profile);
      profile=DestroyStringInfo(profile);
      blob=DetachBlob(buff->blob);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      buff=DestroyImage(buff);
      if (status == MagickFalse)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
    }
  if ((LocaleCompare(image_info->magick,"ICC") == 0) ||
      (LocaleCompare(image_info->magick,"ICM") == 0))
    {
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      blob=(unsigned char *) AcquireQuantumMemory(length,sizeof(unsigned char));
      if (blob == (unsigned char *) NULL)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      AttachBlob(buff->blob,blob,length);
      CopyBlob(image,buff);
      profile=BlobToStringInfo(GetBlobStreamData(buff),(size_t)
        GetBlobSize(buff));
      if (profile == (StringInfo *) NULL)
        {
          blob=DetachBlob(buff->blob);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      (void) SetImageProfile(image,"icc",profile);
      profile=DestroyStringInfo(profile);
      blob=DetachBlob(buff->blob);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      buff=DestroyImage(buff);
    }
  if (LocaleCompare(image_info->magick,"IPTC") == 0)
    {
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      blob=(unsigned char *) AcquireQuantumMemory(length,sizeof(unsigned char));
      if (blob == (unsigned char *) NULL)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      AttachBlob(buff->blob,blob,length);
      CopyBlob(image,buff);
      profile=BlobToStringInfo(GetBlobStreamData(buff),(size_t)
        GetBlobSize(buff));
      if (profile == (StringInfo *) NULL)
        {
          blob=DetachBlob(buff->blob);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      (void) SetImageProfile(image,"iptc",profile);
      profile=DestroyStringInfo(profile);
      blob=DetachBlob(buff->blob);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      buff=DestroyImage(buff);
    }
  if (LocaleCompare(image_info->magick,"XMP") == 0)
    {
      buff=AcquireImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      blob=(unsigned char *) AcquireQuantumMemory(length,sizeof(unsigned char));
      if (blob == (unsigned char *) NULL)
        {
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      AttachBlob(buff->blob,blob,length);
      CopyBlob(image,buff);
      profile=BlobToStringInfo(GetBlobStreamData(buff),(size_t)
        GetBlobSize(buff));
      if (profile == (StringInfo *) NULL)
        {
          blob=DetachBlob(buff->blob);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          buff=DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      (void) SetImageProfile(image,"xmp",profile);
      profile=DestroyStringInfo(profile);
      blob=DetachBlob(buff->blob);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      buff=DestroyImage(buff);
    }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}