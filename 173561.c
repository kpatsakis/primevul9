static inline MagickBooleanType IsGrayColorspace(
  const ColorspaceType colorspace)
{
  if ((colorspace == LinearGRAYColorspace) || (colorspace == GRAYColorspace))
    return(MagickTrue);
  return(MagickFalse);
}