static inline Image *GetImageCache(const ImageInfo *image_info,const char *path,
  ExceptionInfo *exception)
{
  char
    key[MagickPathExtent];

  ExceptionInfo
    *sans_exception;

  Image
    *image;

  ImageInfo
    *read_info;

  /*
    Read an image into a image cache (for repeated usage) if not already in
    cache.  Then return the image that is in the cache.
  */
  (void) FormatLocaleString(key,MagickPathExtent,"cache:%s",path);
  sans_exception=AcquireExceptionInfo();
  image=(Image *) GetImageRegistry(ImageRegistryType,key,sans_exception);
  sans_exception=DestroyExceptionInfo(sans_exception);
  if (image != (Image *) NULL)
    return(image);
  read_info=CloneImageInfo(image_info);
  (void) CopyMagickString(read_info->filename,path,MagickPathExtent);
  image=ReadImage(read_info,exception);
  read_info=DestroyImageInfo(read_info);
  if (image != (Image *) NULL)
    (void) SetImageRegistry(ImageRegistryType,key,image,exception);
  return(image);
}