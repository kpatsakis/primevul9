ModuleExport size_t RegisterPCDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCD");
  entry->decoder=(DecodeImageHandler *) ReadPCDImage;
  entry->encoder=(EncodeImageHandler *) WritePCDImage;
  entry->magick=(IsImageFormatHandler *) IsPCD;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Photo CD");
  entry->module=ConstantString("PCD");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PCDS");
  entry->decoder=(DecodeImageHandler *) ReadPCDImage;
  entry->encoder=(EncodeImageHandler *) WritePCDImage;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Photo CD");
  entry->module=ConstantString("PCD");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}