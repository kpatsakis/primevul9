ModuleExport size_t RegisterSVGImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(LIBXML_DOTTED_VERSION)
  (void) CopyMagickString(version,"XML " LIBXML_DOTTED_VERSION,MaxTextExtent);
#endif
#if defined(MAGICKCORE_RSVG_DELEGATE)
#if !GLIB_CHECK_VERSION(2,35,0)
  g_type_init();
#endif
  (void) FormatLocaleString(version,MaxTextExtent,"RSVG %d.%d.%d",
    LIBRSVG_MAJOR_VERSION,LIBRSVG_MINOR_VERSION,LIBRSVG_MICRO_VERSION);
#endif
#if defined(MAGICKCORE_XML_DELEGATE)
  xmlInitParser();
#endif
  entry=SetMagickInfo("SVG");
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickFalse;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->thread_support=MagickFalse;
#endif
  entry->description=ConstantString("Scalable Vector Graphics");
  entry->mime_type=ConstantString("image/svg+xml");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->magick=(IsImageFormatHandler *) IsSVG;
  entry->module=ConstantString("SVG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("SVGZ");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
#endif
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickFalse;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->thread_support=MagickFalse;
#endif
  entry->description=ConstantString("Compressed Scalable Vector Graphics");
  entry->mime_type=ConstantString("image/svg+xml");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->magick=(IsImageFormatHandler *) IsSVG;
  entry->module=ConstantString("SVG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("MSVG");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
#endif
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickFalse;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->thread_support=MagickFalse;
#endif
  entry->description=ConstantString("ImageMagick's own SVG internal renderer");
  entry->magick=(IsImageFormatHandler *) IsSVG;
  entry->module=ConstantString("SVG");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}