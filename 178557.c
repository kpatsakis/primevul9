static MagickBooleanType IsHDR(const unsigned char *magick,
  const size_t length)
{
  if (length < 10)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"#?RADIANCE",10) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick,"#?RGBE",6) == 0)
    return(MagickTrue);
  return(MagickFalse);
}