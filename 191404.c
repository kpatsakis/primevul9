static inline unsigned int ByteMultiply(const unsigned char alpha,
  const unsigned char beta)
{
  /*
    Byte multiply two elements of GF(2^m) (mix columns and inverse mix columns).
  */
  if ((alpha == 0) || (beta == 0))
    return(0);
  return((unsigned int) InverseLog[(Log[alpha]+Log[beta]) % 0xff]);
}