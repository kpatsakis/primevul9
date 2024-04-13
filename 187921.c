static void TIFFGetEXIFProperties(TIFF *tiff,Image *image,
  const ImageInfo* image_info,ExceptionInfo *exception)
{
  const char
    *option;

  tdir_t
    directory;

#if defined(TIFF_VERSION_BIG)
  uint64
#else
  uint32
#endif
    offset;

  /*
    Read EXIF properties.
  */
  option=GetImageOption(image_info,"tiff:exif-properties");
  if (IsStringFalse(option) != MagickFalse)
    return;
  offset=0;
  if (TIFFGetField(tiff,TIFFTAG_EXIFIFD,&offset) != 1)
    return;
  directory=TIFFCurrentDirectory(tiff);
  if (TIFFReadEXIFDirectory(tiff,offset) == 1)
    TIFFSetImageProperties(tiff,image,"exif:",exception);
  TIFFSetDirectory(tiff,directory);
}