ModuleExport size_t RegisterXCFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("XCF","XCF","GIMP image");
  entry->decoder=(DecodeImageHandler *) ReadXCFImage;
  entry->magick=(IsImageFormatHandler *) IsXCF;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}