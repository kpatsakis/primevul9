MagickPrivate MagickBooleanType TransformImage(Image **image,
  const char *crop_geometry,const char *image_geometry,ExceptionInfo *exception)
{
  Image
    *resize_image,
    *transform_image;

  RectangleInfo
    geometry;

  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickCoreSignature);
  if ((*image)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",(*image)->filename);
  transform_image=(*image);
  if (crop_geometry != (const char *) NULL)
    {
      Image
        *crop_image;

      /*
        Crop image to a user specified size.
      */
      crop_image=CropImageToTiles(*image,crop_geometry,exception);
      if (crop_image == (Image *) NULL)
        transform_image=CloneImage(*image,0,0,MagickTrue,exception);
      else
        {
          transform_image=DestroyImage(transform_image);
          transform_image=GetFirstImageInList(crop_image);
        }
      *image=transform_image;
    }
  if (image_geometry == (const char *) NULL)
    return(MagickTrue);
  /*
    Scale image to a user specified size.
  */
  (void) ParseRegionGeometry(transform_image,image_geometry,&geometry,
    exception);
  if ((transform_image->columns == geometry.width) &&
      (transform_image->rows == geometry.height))
    return(MagickTrue);
  resize_image=ResizeImage(transform_image,geometry.width,geometry.height,
    transform_image->filter,exception);
  if (resize_image == (Image *) NULL)
    return(MagickFalse);
  transform_image=DestroyImage(transform_image);
  transform_image=resize_image;
  *image=transform_image;
  return(MagickTrue);
}