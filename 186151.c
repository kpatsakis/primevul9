ModuleExport size_t RegisterPS3Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS3");
  entry->encoder=(EncodeImageHandler *) WritePS3Image;
  entry->description=ConstantString("Level III Encapsulated PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS3");
  entry->seekable_stream=MagickTrue;
  entry->blob_support=MagickFalse;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS3");
  entry->encoder=(EncodeImageHandler *) WritePS3Image;
  entry->description=ConstantString("Level III PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS3");
  entry->seekable_stream=MagickTrue;
  entry->blob_support=MagickFalse;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}