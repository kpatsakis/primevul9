static void TIFFGetGPSProperties(TIFF *tiff,Image *image,
  const ImageInfo* image_info,ExceptionInfo *exception)
{
#if (TIFFLIB_VERSION >= 20210416)
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
    Read GPS properties.
  */
  option=GetImageOption(image_info,"tiff:gps-properties");
  if (IsStringFalse(option) != MagickFalse)
    return;
  offset=0;
  if (TIFFGetField(tiff,TIFFTAG_GPSIFD,&offset) != 1)
    return;
  directory=TIFFCurrentDirectory(tiff);
  if (TIFFReadGPSDirectory(tiff,offset) == 1)
    TIFFSetImageProperties(tiff,image,"exif:GPS",exception);
  TIFFSetDirectory(tiff,directory);
#else
  magick_unreferenced(tiff);
  magick_unreferenced(image);
  magick_unreferenced(image_info);
  magick_unreferenced(exception);
#endif
}