static inline void AlphaBlendPixelInfo(const Image *image,
  const Quantum *pixel,PixelInfo *pixel_info,double *alpha)
{
  if (image->alpha_trait == UndefinedPixelTrait)
    {
      *alpha=1.0;
      pixel_info->red=(double) GetPixelRed(image,pixel);
      pixel_info->green=(double) GetPixelGreen(image,pixel);
      pixel_info->blue=(double) GetPixelBlue(image,pixel);
      pixel_info->black=0.0;
      if (image->colorspace == CMYKColorspace)
        pixel_info->black=(double) GetPixelBlack(image,pixel);
      pixel_info->alpha=(double) GetPixelAlpha(image,pixel);
      return;
    }
  *alpha=QuantumScale*GetPixelAlpha(image,pixel);
  pixel_info->red=(*alpha*GetPixelRed(image,pixel));
  pixel_info->green=(*alpha*GetPixelGreen(image,pixel));
  pixel_info->blue=(*alpha*GetPixelBlue(image,pixel));
  pixel_info->black=0.0;
  if (image->colorspace == CMYKColorspace)
    pixel_info->black=(*alpha*GetPixelBlack(image,pixel));
  pixel_info->alpha=(double) GetPixelAlpha(image,pixel);
}