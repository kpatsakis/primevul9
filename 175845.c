static MagickBooleanType IsPES(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"#PES",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}