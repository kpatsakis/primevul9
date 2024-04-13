static MagickBooleanType IsVIPS(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);

  if (memcmp(magick,"\010\362\246\266",4) == 0)
    return(MagickTrue);

  if (memcmp(magick,"\266\246\362\010",4) == 0)
    return(MagickTrue);

  return(MagickFalse);
}