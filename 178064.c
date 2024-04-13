ModuleExport size_t RegisterSGIImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("SGI","SGI","Irix RGB image");
  entry->decoder=(DecodeImageHandler *) ReadSGIImage;
  entry->encoder=(EncodeImageHandler *) WriteSGIImage;
  entry->magick=(IsImageFormatHandler *) IsSGI;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}