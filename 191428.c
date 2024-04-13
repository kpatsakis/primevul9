static inline unsigned int XTime(unsigned char alpha)
{
  unsigned char
    beta;

  beta=(unsigned char) ((alpha & 0x80) != 0 ? 0x1b : 0);
  alpha<<=1;
  alpha^=beta;
  return(alpha);
}