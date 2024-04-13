static inline unsigned int RotateRight(const unsigned int x)
{
  return((x >> 8) | ((x & 0xff) << 24));
}