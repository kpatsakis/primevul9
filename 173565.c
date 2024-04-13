static inline MagickBooleanType IssRGBCompatibleColorspace(
  const ColorspaceType colorspace)
{
  if ((colorspace == sRGBColorspace) || (colorspace == RGBColorspace) ||
      (colorspace == scRGBColorspace) || (colorspace == TransparentColorspace) ||
      (colorspace == GRAYColorspace) || (colorspace == LinearGRAYColorspace))
    return(MagickTrue);
  return(MagickFalse);
}