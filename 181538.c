MagickExport Image *ReadInlineImage(const ImageInfo *image_info,
  const char *content,ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *read_info;

  unsigned char
    *blob;

  size_t
    length;

  register const char
    *p;

  /*
    Skip over header (e.g. data:image/gif;base64,).
  */
  image=NewImageList();
  for (p=content; (*p != ',') && (*p != '\0'); p++) ;
  if (*p == '\0')
    ThrowReaderException(CorruptImageError,"CorruptImage");
  p++;
  length=0;
  blob=Base64Decode(p,&length);
  if (length == 0)
    ThrowReaderException(CorruptImageError,"CorruptImage");
  read_info=CloneImageInfo(image_info);
  (void) SetImageInfoProgressMonitor(read_info,(MagickProgressMonitor) NULL,
    (void *) NULL);
  *read_info->filename='\0';
  *read_info->magick='\0';
  image=BlobToImage(read_info,blob,length,exception);
  blob=(unsigned char *) RelinquishMagickMemory(blob);
  read_info=DestroyImageInfo(read_info);
  return(image);
}