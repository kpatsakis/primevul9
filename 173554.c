static inline MagickBooleanType IsRGBColorspace(const ColorspaceType colorspace)
{
  if ((colorspace == RGBColorspace) || (colorspace == scRGBColorspace) ||
      (colorspace == LinearGRAYColorspace))
    return(MagickTrue);
  return(MagickFalse);
}