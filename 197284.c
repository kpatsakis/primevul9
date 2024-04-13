ModuleExport size_t RegisterDPSImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("DPS","DPS","Display Postscript Interpreter");
#if defined(MAGICKCORE_DPS_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDPSImage;
#endif
  entry->flags^=CoderBlobSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}