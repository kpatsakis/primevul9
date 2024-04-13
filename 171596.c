static inline const unsigned char *PushLongPixel(const EndianType endian,
  const unsigned char *pixels,unsigned int *pixel)
{
  register unsigned int
    quantum;

  if (endian == LSBEndian)
    {
      quantum=((unsigned int) *pixels++);
      quantum|=((unsigned int) *pixels++ << 8);
      quantum|=((unsigned int) *pixels++ << 16);
      quantum|=((unsigned int) *pixels++ << 24);
      *pixel=quantum;
      return(pixels);
    }
  quantum=((unsigned int) *pixels++ << 24);
  quantum|=((unsigned int) *pixels++ << 16);
  quantum|=((unsigned int) *pixels++ << 8);
  quantum|=((unsigned int) *pixels++);
  *pixel=quantum;
  return(pixels);
}