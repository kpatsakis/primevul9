MagickExport Image *TrimImage(const Image *image,ExceptionInfo *exception)
{
  RectangleInfo
    geometry;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  geometry=GetImageBoundingBox(image,exception);
  if ((geometry.width == 0) || (geometry.height == 0))
    {
      Image
        *crop_image;

      crop_image=CloneImage(image,1,1,MagickTrue,exception);
      if (crop_image == (Image *) NULL)
        return((Image *) NULL);
      crop_image->background_color.alpha=(MagickRealType) TransparentAlpha;
      crop_image->alpha_trait=BlendPixelTrait;
      (void) SetImageBackgroundColor(crop_image,exception);
      crop_image->page=image->page;
      crop_image->page.x=(-1);
      crop_image->page.y=(-1);
      return(crop_image);
    }
  geometry.x+=image->page.x;
  geometry.y+=image->page.y;
  return(CropImage(image,&geometry,exception));
}