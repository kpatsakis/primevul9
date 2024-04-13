ModuleExport size_t RegisterCAPTIONImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("CAPTION","CAPTION","Caption");
  entry->decoder=(DecodeImageHandler *) ReadCAPTIONImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}