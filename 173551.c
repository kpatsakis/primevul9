static inline MagickBooleanType IsCMYKColorspace(
  const ColorspaceType colorspace)
{
  if (colorspace == CMYKColorspace)
    return(MagickTrue);
  return(MagickFalse);
}