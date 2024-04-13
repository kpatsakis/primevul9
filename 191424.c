static inline unsigned int ByteSubTransform(unsigned int x,
  unsigned char *s_box)
{
  unsigned int
    key;

  /*
    Non-linear layer resists differential and linear cryptoanalysis attacks.
  */
  key=((unsigned int) s_box[x & 0xff]) |
    ((unsigned int) s_box[(x >> 8) & 0xff] << 8) |
    ((unsigned int) s_box[(x >> 16) & 0xff] << 16) |
    ((unsigned int) s_box[(x >> 24) & 0xff] << 24);
  return(key);
}