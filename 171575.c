static inline unsigned char *PopCharPixel(const unsigned char pixel,
  unsigned char *pixels)
{
  *pixels++=pixel;
  return(pixels);
}