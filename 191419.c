static inline void InverseAddRoundKey(const unsigned int *alpha,
  unsigned int *beta)
{
  unsigned int
    i,
    j;

  for (i=0; i < 4; i++)
  {
    beta[i]=0;
    for (j=0; j < 4; j++)
      beta[i]|=(ByteMultiply(0xe,(alpha[i] >> (8*j)) & 0xff) ^
        ByteMultiply(0xb,(alpha[i] >> (8*((j+1) % 4))) & 0xff) ^
        ByteMultiply(0xd,(alpha[i] >> (8*((j+2) % 4))) & 0xff) ^
        ByteMultiply(0x9,(alpha[i] >> (8*((j+3) % 4))) & 0xff)) << (8*j);
  }
}