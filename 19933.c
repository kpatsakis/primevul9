ModuleExport size_t RegisterCINImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("CIN","CIN","Cineon Image File");
  entry->decoder=(DecodeImageHandler *) ReadCINImage;
  entry->encoder=(EncodeImageHandler *) WriteCINImage;
  entry->magick=(IsImageFormatHandler *) IsCIN;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}