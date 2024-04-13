static inline MagickBooleanType IssRGBColorspace(
  const ColorspaceType colorspace)
{
  if ((colorspace == sRGBColorspace) || (colorspace == TransparentColorspace))
    return(MagickTrue);
  return(MagickFalse);
}