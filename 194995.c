ModuleExport size_t RegisterPANGOImage(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(PANGO_VERSION_STRING)
  (void) FormatLocaleString(version,MagickPathExtent,"Pangocairo %s",
    PANGO_VERSION_STRING);
#endif
  entry=AcquireMagickInfo("PANGO","PANGO","Pango Markup Language");
#if defined(MAGICKCORE_PANGOCAIRO_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPANGOImage;
#endif
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->flags^=CoderAdjoinFlag;
  entry->flags^=CoderDecoderThreadSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}