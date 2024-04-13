ModuleExport size_t RegisterPDFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AI");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->adjoin=MagickFalse;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Adobe Illustrator CS2");
  entry->mime_type=ConstantString("application/pdf");
  entry->module=ConstantString("PDF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("EPDF");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->adjoin=MagickFalse;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Encapsulated Portable Document Format");
  entry->mime_type=ConstantString("application/pdf");
  entry->module=ConstantString("PDF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PDF");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->magick=(IsImageFormatHandler *) IsPDF;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Portable Document Format");
  entry->mime_type=ConstantString("application/pdf");
  entry->module=ConstantString("PDF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PDFA");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->magick=(IsImageFormatHandler *) IsPDF;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Portable Document Archive Format");
  entry->mime_type=ConstantString("application/pdf");
  entry->module=ConstantString("PDF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}