ModuleExport size_t RegisterPESImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PES","PES","Embrid Embroidery Format");
  entry->decoder=(DecodeImageHandler *) ReadPESImage;
  entry->magick=(IsImageFormatHandler *) IsPES;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}