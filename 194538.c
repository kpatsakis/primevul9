ModuleExport size_t RegisterCINImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CIN");
  entry->decoder=(DecodeImageHandler *) ReadCINImage;
  entry->encoder=(EncodeImageHandler *) WriteCINImage;
  entry->magick=(IsImageFormatHandler *) IsCIN;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Cineon Image File");
  entry->module=ConstantString("CIN");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}