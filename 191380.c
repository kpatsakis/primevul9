static inline void AddRoundKey(const unsigned int *ciphertext,
  const unsigned int *key,unsigned int *plaintext)
{
  ssize_t
    i;

  /*
    Xor corresponding text input and round key input bytes.
  */
  for (i=0; i < 4; i++)
    plaintext[i]=key[i] ^ ciphertext[i];
}