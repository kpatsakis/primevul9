static inline MagickBooleanType IsAuthenticPixel(const Image *image,
  const ssize_t x,const ssize_t y)
{
  if ((x < 0) || (x >= (ssize_t) image->columns))
    return(MagickFalse);
  if ((y < 0) || (y >= (ssize_t) image->rows))
    return(MagickFalse);
  return(MagickTrue);
}