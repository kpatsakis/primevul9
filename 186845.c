ModuleExport size_t RegisterPANGOImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(PANGO_VERSION_STRING)
  (void) FormatLocaleString(version,MaxTextExtent,"Pangocairo %s",
    PANGO_VERSION_STRING);
#endif
  entry=SetMagickInfo("PANGO");
#if defined(MAGICKCORE_PANGOCAIRO_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPANGOImage;
#endif
  entry->description=ConstantString("Pango Markup Language");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->adjoin=MagickFalse;
  entry->thread_support=MagickFalse;
  entry->module=ConstantString("PANGO");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}