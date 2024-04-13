static inline void IncrementCipherNonce(const size_t length,
  unsigned char *nonce)
{
  ssize_t
    i;

  for (i=(ssize_t) (length-1); i >= 0; i--)
  {
    nonce[i]++;
    if (nonce[i] != 0)
      return;
  }
  ThrowFatalException(ResourceLimitFatalError,"Sequence wrap error `%s'");
}