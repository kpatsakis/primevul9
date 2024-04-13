ModuleExport size_t RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("ICON","CUR","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("ICON","ICO","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("ICON","ICON","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}