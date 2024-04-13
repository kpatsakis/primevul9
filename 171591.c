static inline const unsigned char *PushCharPixel(const unsigned char *pixels,
  unsigned char *pixel)
{
  *pixel=(*pixels++);
  return(pixels);
}