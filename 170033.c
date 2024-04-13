ModuleExport size_t RegisterMIFFImage(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(MagickImageCoderSignatureText)
  (void) CopyMagickString(version,MagickLibVersionText,MagickPathExtent);
#if defined(ZLIB_VERSION)
  (void) ConcatenateMagickString(version," with Zlib ",MagickPathExtent);
  (void) ConcatenateMagickString(version,ZLIB_VERSION,MagickPathExtent);
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
  (void) ConcatenateMagickString(version," and BZlib",MagickPathExtent);
#endif
#endif
  entry=AcquireMagickInfo("MIFF","MIFF","Magick Image File Format");
  entry->decoder=(DecodeImageHandler *) ReadMIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteMIFFImage;
  entry->magick=(IsImageFormatHandler *) IsMIFF;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  if (*version != '\0')
    entry->version=ConstantString(version);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}