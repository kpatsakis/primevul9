static inline MagickBooleanType IsSameColor(const Image *image,
  const PixelPacket *p,const PixelPacket *q)
{
  if ((GetPixelRed(p) != GetPixelRed(q)) ||
      (GetPixelGreen(p) != GetPixelGreen(q)) ||
      (GetPixelBlue(p) != GetPixelBlue(q)))
    return(MagickFalse);
  if ((image->matte != MagickFalse) &&
      (GetPixelOpacity(p) != GetPixelOpacity(q)))
    return(MagickFalse);
  return(MagickTrue);
}