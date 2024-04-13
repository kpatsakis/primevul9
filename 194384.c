static inline unsigned char *PopHexPixel(const char **hex_digits,
  const size_t pixel,unsigned char *pixels)
{
  register const char
    *hex;

  hex=hex_digits[pixel];
  *pixels++=(unsigned char) (*hex++);
  *pixels++=(unsigned char) (*hex);
  return(pixels);
}