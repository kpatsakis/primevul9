ModuleExport size_t RegisterJP2Image(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  (void) FormatLocaleString(version,MaxTextExtent,"%s",opj_version());
#endif
  entry=SetMagickInfo("JP2");
  entry->description=ConstantString("JPEG-2000 File Format Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("J2C");
  entry->description=ConstantString("JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJ2K;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("J2K");
  entry->description=ConstantString("JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJ2K;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPM");
  entry->description=ConstantString("JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPT");
  entry->description=ConstantString("JPEG-2000 File Format Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPC");
  entry->description=ConstantString("JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->module=ConstantString("JP2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}