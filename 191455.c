ModuleExport size_t RegisterTHUMBNAILImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("THUMBNAIL","THUMBNAIL","EXIF Profile Thumbnail");
  entry->encoder=(EncodeImageHandler *) WriteTHUMBNAILImage;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}