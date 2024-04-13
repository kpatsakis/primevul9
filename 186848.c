ModuleExport size_t RegisterVIDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VID");
  entry->decoder=(DecodeImageHandler *) ReadVIDImage;
  entry->encoder=(EncodeImageHandler *) WriteVIDImage;
  entry->format_type=ImplicitFormatType;
  entry->description=ConstantString("Visual Image Directory");
  entry->module=ConstantString("VID");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}