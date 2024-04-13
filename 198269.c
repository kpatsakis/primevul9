ModuleExport size_t RegisterDIBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DIB");
  entry->decoder=(DecodeImageHandler *) ReadDIBImage;
  entry->encoder=(EncodeImageHandler *) WriteDIBImage;
  entry->magick=(IsImageFormatHandler *) IsDIB;
  entry->adjoin=MagickFalse;
  entry->stealth=MagickTrue;
  entry->description=ConstantString(
    "Microsoft Windows 3.X Packed Device-Independent Bitmap");
  entry->module=ConstantString("DIB");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("ICODIB");
  entry->decoder=(DecodeImageHandler *) ReadDIBImage;
  entry->magick=(IsImageFormatHandler *) IsDIB;
  entry->adjoin=MagickFalse;
  entry->stealth=MagickTrue;
  entry->description=ConstantString(
    "Microsoft Windows 3.X Packed Device-Independent Bitmap");
  entry->module=ConstantString("DIB");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}