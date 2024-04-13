ModuleExport size_t RegisterMSLImage(void)
{
  MagickInfo
    *entry;

#if defined(MAGICKCORE_XML_DELEGATE)
  xmlInitParser();
#endif
  entry=SetMagickInfo("MSL");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadMSLImage;
  entry->encoder=(EncodeImageHandler *) WriteMSLImage;
#endif
  entry->format_type=ImplicitFormatType;
  entry->description=ConstantString("Magick Scripting Language");
  entry->module=ConstantString("MSL");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}