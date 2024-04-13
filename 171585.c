static inline unsigned char *PopLongPixel(const EndianType endian,
  const unsigned int pixel,unsigned char *pixels)
{
  register unsigned int
    quantum;

  quantum=(unsigned int) pixel;
  if (endian == LSBEndian)
    {
      *pixels++=(unsigned char) (quantum);
      *pixels++=(unsigned char) (quantum >> 8);
      *pixels++=(unsigned char) (quantum >> 16);
      *pixels++=(unsigned char) (quantum >> 24);
      return(pixels);
    }
  *pixels++=(unsigned char) (quantum >> 24);
  *pixels++=(unsigned char) (quantum >> 16);
  *pixels++=(unsigned char) (quantum >> 8);
  *pixels++=(unsigned char) (quantum);
  return(pixels);
}