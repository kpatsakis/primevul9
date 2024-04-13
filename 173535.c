static unsigned char *EncodeRLE(unsigned char *destination,
  unsigned char *source,size_t literal,size_t repeat)
{
  if (literal > 0)
    *destination++=(unsigned char) (literal-1);
  (void) memcpy(destination,source,literal);
  destination+=literal;
  if (repeat > 0)
    {
      *destination++=(unsigned char) (0x80 | (repeat-1));
      *destination++=source[literal];
    }
  return(destination);
}