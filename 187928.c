ModuleExport size_t RegisterTIFFImage(void)
{
#define TIFFDescription  "Tagged Image File Format"

  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

#if defined(MAGICKCORE_TIFF_DELEGATE)
  if (tiff_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&tiff_semaphore);
  LockSemaphoreInfo(tiff_semaphore);
  if (instantiate_key == MagickFalse)
    {
      if (CreateMagickThreadKey(&tiff_exception,NULL) == MagickFalse)
        ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
      error_handler=TIFFSetErrorHandler(TIFFErrors);
      warning_handler=TIFFSetWarningHandler(TIFFWarnings);
      if (tag_extender == (TIFFExtendProc) NULL)
        tag_extender=TIFFSetTagExtender(TIFFTagExtender);
      instantiate_key=MagickTrue;
    }
  UnlockSemaphoreInfo(tiff_semaphore);
#endif
  *version='\0';
#if defined(TIFF_VERSION)
  (void) FormatLocaleString(version,MagickPathExtent,"%d",TIFF_VERSION);
#endif
#if defined(MAGICKCORE_TIFF_DELEGATE)
  {
    const char
      *p;

    ssize_t
      i;

    p=TIFFGetVersion();
    for (i=0; (i < (MagickPathExtent-1)) && (*p != 0) && (*p != '\n'); i++)
      version[i]=(*p++);
    version[i]='\0';
  }
#endif

  entry=AcquireMagickInfo("TIFF","GROUP4","Raw CCITT Group4");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadGROUP4Image;
  entry->encoder=(EncodeImageHandler *) WriteGROUP4Image;
#endif
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  entry->flags^=CoderUseExtensionFlag;
  entry->format_type=ImplicitFormatType;
  entry->mime_type=ConstantString("image/tiff");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TIFF","PTIF","Pyramid encoded TIFF");
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WritePTIFImage;
#endif
  entry->flags|=CoderEndianSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderUseExtensionFlag;
  entry->mime_type=ConstantString("image/tiff");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TIFF","TIF",TIFFDescription);
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->flags|=CoderEndianSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags|=CoderStealthFlag;
  entry->flags^=CoderUseExtensionFlag;
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TIFF","TIFF",TIFFDescription);
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->magick=(IsImageFormatHandler *) IsTIFF;
  entry->flags|=CoderEndianSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderUseExtensionFlag;
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TIFF","TIFF64","Tagged Image File Format (64-bit)");
#if defined(TIFF_VERSION_BIG)
  entry->decoder=(DecodeImageHandler *) ReadTIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteTIFFImage;
#endif
  entry->flags|=CoderEndianSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderUseExtensionFlag;
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/tiff");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}