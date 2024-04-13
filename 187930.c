static MagickBooleanType GetTIFFInfo(const ImageInfo *image_info,
  TIFF *tiff,TIFFInfo *tiff_info)
{
#define TIFFStripSizeDefault  1048576

  const char
    *option;

  MagickStatusType
    flags;

  uint32
    tile_columns,
    tile_rows;

  assert(tiff_info != (TIFFInfo *) NULL);
  (void) memset(tiff_info,0,sizeof(*tiff_info));
  option=GetImageOption(image_info,"tiff:tile-geometry");
  if (option == (const char *) NULL)
    {
      size_t
        extent;

      uint32
        rows,
        rows_per_strip;

      extent=TIFFScanlineSize(tiff);
      rows_per_strip=TIFFStripSizeDefault/(extent == 0 ? 1 : (uint32) extent);
      rows_per_strip=16*(((rows_per_strip < 16 ? 16 : rows_per_strip)+1)/16);
      if ((TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&rows) == 1) &&
          (rows_per_strip > rows))
        rows_per_strip=rows;
      option=GetImageOption(image_info,"tiff:rows-per-strip");
      if (option != (const char *) NULL)
        rows_per_strip=(uint32) strtoul(option,(char **) NULL,10);
      rows_per_strip=TIFFDefaultStripSize(tiff,rows_per_strip);
      (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
      return(MagickTrue);
    }
  /*
    Create tiled TIFF, ignore "tiff:rows-per-strip".
  */
  flags=ParseAbsoluteGeometry(option,&tiff_info->tile_geometry);
  if ((flags & HeightValue) == 0)
    tiff_info->tile_geometry.height=tiff_info->tile_geometry.width;
  tile_columns=(uint32) tiff_info->tile_geometry.width;
  tile_rows=(uint32) tiff_info->tile_geometry.height;
  TIFFDefaultTileSize(tiff,&tile_columns,&tile_rows);
  (void) TIFFSetField(tiff,TIFFTAG_TILEWIDTH,tile_columns);
  (void) TIFFSetField(tiff,TIFFTAG_TILELENGTH,tile_rows);
  tiff_info->tile_geometry.width=tile_columns;
  tiff_info->tile_geometry.height=tile_rows;
  if ((TIFFScanlineSize(tiff) <= 0) || (TIFFTileSize(tiff) <= 0))
    {
      DestroyTIFFInfo(tiff_info);
      return(MagickFalse);
    }
  tiff_info->scanlines=(unsigned char *) AcquireQuantumMemory((size_t)
    tile_rows*TIFFScanlineSize(tiff),sizeof(*tiff_info->scanlines));
  tiff_info->pixels=(unsigned char *) AcquireQuantumMemory((size_t)
    tile_rows*TIFFTileSize(tiff),sizeof(*tiff_info->scanlines));
  if ((tiff_info->scanlines == (unsigned char *) NULL) ||
      (tiff_info->pixels == (unsigned char *) NULL))
    {
      DestroyTIFFInfo(tiff_info);
      return(MagickFalse);
    }
  return(MagickTrue);
}