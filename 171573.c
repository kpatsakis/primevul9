static inline unsigned char *PopShortPixel(const EndianType endian,
  const unsigned short pixel,unsigned char *pixels)
{
  register unsigned int
    quantum;

  quantum=pixel;
  if (endian == LSBEndian)
    {
      *pixels++=(unsigned char) (quantum);
      *pixels++=(unsigned char) (quantum >> 8);
      return(pixels);
    }
  *pixels++=(unsigned char) (quantum >> 8);
  *pixels++=(unsigned char) (quantum);
  return(pixels);
}