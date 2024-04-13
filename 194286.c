ModuleExport size_t RegisterDOTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DOT");
#if defined(MAGICKCORE_GVC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDOTImage;
#endif
  entry->blob_support=MagickFalse;
  entry->description=ConstantString("Graphviz");
  entry->module=ConstantString("DOT");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("GV");
#if defined(MAGICKCORE_GVC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDOTImage;
#endif
  entry->blob_support=MagickFalse;
  entry->description=ConstantString("Graphviz");
  entry->module=ConstantString("DOT");
  (void) RegisterMagickInfo(entry);
#if defined(MAGICKCORE_GVC_DELEGATE)
  graphic_context=gvContext();
#endif
  return(MagickImageCoderSignature);
}