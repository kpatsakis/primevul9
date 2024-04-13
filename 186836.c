ModuleExport size_t RegisterGIFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("GIF","GIF",
    "CompuServe graphics interchange format");
  entry->decoder=(DecodeImageHandler *) ReadGIFImage;
  entry->encoder=(EncodeImageHandler *) WriteGIFImage;
  entry->magick=(IsImageFormatHandler *) IsGIF;
  entry->mime_type=ConstantString("image/gif");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("GIF","GIF87",
    "CompuServe graphics interchange format");
  entry->decoder=(DecodeImageHandler *) ReadGIFImage;
  entry->encoder=(EncodeImageHandler *) WriteGIFImage;
  entry->magick=(IsImageFormatHandler *) IsGIF;
  entry->flags^=CoderAdjoinFlag;
  entry->version=ConstantString("version 87a");
  entry->mime_type=ConstantString("image/gif");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}