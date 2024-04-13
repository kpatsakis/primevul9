static inline void AssociateAlphaPixel(const CubeInfo *cube_info,
  const PixelPacket *pixel,DoublePixelPacket *alpha_pixel)
{
  MagickRealType
    alpha;

  alpha_pixel->index=0;
  if ((cube_info->associate_alpha == MagickFalse) ||
      (pixel->opacity == OpaqueOpacity))
    {
      alpha_pixel->red=(MagickRealType) GetPixelRed(pixel);
      alpha_pixel->green=(MagickRealType) GetPixelGreen(pixel);
      alpha_pixel->blue=(MagickRealType) GetPixelBlue(pixel);
      alpha_pixel->opacity=(MagickRealType) GetPixelOpacity(pixel);
      return;
    }
  alpha=(MagickRealType) (QuantumScale*(QuantumRange-GetPixelOpacity(pixel)));
  alpha_pixel->red=alpha*GetPixelRed(pixel);
  alpha_pixel->green=alpha*GetPixelGreen(pixel);
  alpha_pixel->blue=alpha*GetPixelBlue(pixel);
  alpha_pixel->opacity=(MagickRealType) GetPixelOpacity(pixel);
}