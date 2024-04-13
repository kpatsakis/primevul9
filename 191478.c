static void InitializeRoundKey(const unsigned char *ciphertext,
  const unsigned int *key,unsigned int *plaintext)
{
  const unsigned char
    *p;

  unsigned int
    i,
    j;

  unsigned int
    value;

  p=ciphertext;
  for (i=0; i < 4; i++)
  {
    value=0;
    for (j=0; j < 4; j++)
      value|=((unsigned int) *p++ << (8*j));
    plaintext[i]=key[i] ^ value;
  }
  /*
    Reset registers.
  */
  value=0;
}