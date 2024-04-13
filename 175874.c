ModuleExport size_t RegisterDIBImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("DIB","DIB",
    "Microsoft Windows 3.X Packed Device-Independent Bitmap");
  entry->decoder=(DecodeImageHandler *) ReadDIBImage;
  entry->encoder=(EncodeImageHandler *) WriteDIBImage;
  entry->magick=(IsImageFormatHandler *) IsDIB;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderStealthFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("DIB","ICODIB",
    "Microsoft Windows 3.X Packed Device-Independent Bitmap");
  entry->decoder=(DecodeImageHandler *) ReadDIBImage;
  entry->magick=(IsImageFormatHandler *) IsDIB;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderStealthFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}