ModuleExport size_t RegisterVIPSImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("VIPS","VIPS","VIPS image");
  entry->decoder=(DecodeImageHandler *) ReadVIPSImage;
  entry->encoder=(EncodeImageHandler *) WriteVIPSImage;
  entry->magick=(IsImageFormatHandler *) IsVIPS;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}