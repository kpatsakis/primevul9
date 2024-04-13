ModuleExport size_t RegisterPCLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCL");
  entry->decoder=(DecodeImageHandler *) ReadPCLImage;
  entry->encoder=(EncodeImageHandler *) WritePCLImage;
  entry->magick=(IsImageFormatHandler *) IsPCL;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->thread_support=EncoderThreadSupport;
  entry->description=ConstantString("Printer Control Language");
  entry->module=ConstantString("PCL");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}