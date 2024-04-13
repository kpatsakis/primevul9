ModuleExport size_t RegisterVIFFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("VIFF","VIFF","Khoros Visualization image");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  entry->magick=(IsImageFormatHandler *) IsVIFF;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("VIFF","XV","Khoros Visualization image");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}