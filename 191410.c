static inline unsigned int RotateLeft(const unsigned int x)
{
  return(((x << 8) | ((x >> 24) & 0xff)));
}