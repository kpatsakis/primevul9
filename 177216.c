static MagickBooleanType IsXCF(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick,"gimp xcf",8) == 0)
    return(MagickTrue);
  return(MagickFalse);
}