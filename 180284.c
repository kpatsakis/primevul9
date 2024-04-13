static inline unsigned char *PopHexPixel(const char hex_digits[][3],
  const size_t pixel,unsigned char *pixels)
{
  register const char
    *hex;

  hex=hex_digits[pixel];
  *pixels++=(unsigned char) (*hex++ & 0xff);
  *pixels++=(unsigned char) (*hex & 0xff);
  return(pixels);
}