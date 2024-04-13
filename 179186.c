ModuleExport size_t RegisterPNMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PAM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->description=ConstantString("Common 2-dimensional bitmap format");
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  entry->module=ConstantString("PNM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PBM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->description=ConstantString("Portable bitmap format (black and white)");
  entry->mime_type=ConstantString("image/x-portable-bitmap");
  entry->module=ConstantString("PNM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PFM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Portable float format");
  entry->module=ConstantString("PFM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PGM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->description=ConstantString("Portable graymap format (gray scale)");
  entry->mime_type=ConstantString("image/x-portable-greymap");
  entry->module=ConstantString("PNM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PNM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->magick=(IsImageFormatHandler *) IsPNM;
  entry->description=ConstantString("Portable anymap");
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  entry->module=ConstantString("PNM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PPM");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->description=ConstantString("Portable pixmap format (color)");
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  entry->module=ConstantString("PNM");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}