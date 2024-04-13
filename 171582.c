static inline const unsigned char *PushShortPixel(const EndianType endian,
  const unsigned char *pixels,unsigned short *pixel)
{
  register unsigned int
    quantum;

  if (endian == LSBEndian)
    {
      quantum=(unsigned int) *pixels++;
      quantum|=(unsigned int) (*pixels++ << 8);
      *pixel=(unsigned short) (quantum & 0xffff);
      return(pixels);
    }
  quantum=(unsigned int) (*pixels++ << 8);
  quantum|=(unsigned int) *pixels++;
  *pixel=(unsigned short) (quantum & 0xffff);
  return(pixels);
}