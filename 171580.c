static inline const unsigned char *PushFloatPixel(const EndianType endian,
  const unsigned char *pixels,MagickFloatType *pixel)
{
  union
  {
    unsigned int
      unsigned_value;

    MagickFloatType
      float_value;
  } quantum;

  if (endian == LSBEndian)
    {
      quantum.unsigned_value=((unsigned int) *pixels++);
      quantum.unsigned_value|=((unsigned int) *pixels++ << 8);
      quantum.unsigned_value|=((unsigned int) *pixels++ << 16);
      quantum.unsigned_value|=((unsigned int) *pixels++ << 24);
      *pixel=quantum.float_value;
      return(pixels);
    }
  quantum.unsigned_value=((unsigned int) *pixels++ << 24);
  quantum.unsigned_value|=((unsigned int) *pixels++ << 16);
  quantum.unsigned_value|=((unsigned int) *pixels++ << 8);
  quantum.unsigned_value|=((unsigned int) *pixels++);
  *pixel=quantum.float_value;
  return(pixels);
}