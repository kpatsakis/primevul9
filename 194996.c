ModuleExport size_t RegisterVIDImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("VID","VID","Visual Image Directory");
  entry->decoder=(DecodeImageHandler *) ReadVIDImage;
  entry->encoder=(EncodeImageHandler *) WriteVIDImage;
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}