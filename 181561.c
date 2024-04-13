MagickExport Image *ConstituteImage(const size_t columns,const size_t rows,
  const char *map,const StorageType storage,const void *pixels,
  ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  /*
    Allocate image structure.
  */
  assert(map != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",map);
  assert(pixels != (void *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage((ImageInfo *) NULL,exception);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(OptionError,"NonZeroWidthAndHeightRequired");
  image->columns=columns;
  image->rows=rows;
  (void) SetImageBackgroundColor(image,exception);
  status=ImportImagePixels(image,0,0,columns,rows,map,storage,pixels,exception);
  if (status == MagickFalse)
     image=DestroyImage(image);
  return(image);
}