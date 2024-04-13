ModuleExport size_t RegisterMATImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MAT");
  entry->decoder=(DecodeImageHandler *) ReadMATImage;
  entry->encoder=(EncodeImageHandler *) WriteMATImage;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=AcquireString("MATLAB level 5 image format");
  entry->module=AcquireString("MAT");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}