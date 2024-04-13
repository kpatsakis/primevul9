MagickExport void ConformPixelInfo(Image *image,const PixelInfo *source,
  PixelInfo *destination,ExceptionInfo *exception)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(destination != (const PixelInfo *) NULL);
  *destination=(*source);
  if (image->colorspace == CMYKColorspace)
    {
      if (IssRGBCompatibleColorspace(destination->colorspace))
        ConvertRGBToCMYK(destination);
    }
  else
    if (destination->colorspace == CMYKColorspace)
      {
        if (IssRGBCompatibleColorspace(image->colorspace))
          ConvertCMYKToRGB(destination);
      }
  if ((IsPixelInfoGray(&image->background_color) == MagickFalse) &&
      (IsGrayColorspace(image->colorspace) != MagickFalse))
    (void) TransformImageColorspace(image,sRGBColorspace,exception);
  if ((destination->alpha_trait != UndefinedPixelTrait) &&
      (image->alpha_trait == UndefinedPixelTrait))
    (void) SetImageAlpha(image,OpaqueAlpha,exception);
}