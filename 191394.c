static void FinalizeRoundKey(const unsigned int *ciphertext,
  const unsigned int *key,unsigned char *plaintext)
{
  unsigned char
    *p;

  unsigned int
    i,
    j;

  unsigned int
    value;

  /*
    The round key is XORed with the result of the mix-column transformation.
  */
  p=plaintext;
  for (i=0; i < 4; i++)
  {
    value=ciphertext[i] ^ key[i];
    for (j=0; j < 4; j++)
      *p++=(unsigned char) ((value >> (8*j)) & 0xff);
  }
  /*
    Reset registers.
  */
  value=0;
}