static void SetAESKey(AESInfo *aes_info,const StringInfo *key)
{
  ssize_t
    i;

  ssize_t
    bytes,
    n;

  unsigned char
    *datum;

  unsigned int
    alpha,
    beta;

  /*
    Determine the number of rounds based on the number of bits in key.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(aes_info != (AESInfo *) NULL);
  assert(aes_info->signature == MagickCoreSignature);
  assert(key != (StringInfo *) NULL);
  n=4;
  aes_info->rounds=10;
  if ((8*GetStringInfoLength(key)) >= 256)
    {
      n=8;
      aes_info->rounds=14;
    }
  else
    if ((8*GetStringInfoLength(key)) >= 192)
      {
        n=6;
        aes_info->rounds=12;
      }
  /*
    Generate crypt key.
  */
  datum=GetStringInfoDatum(aes_info->key);
  (void) memset(datum,0,GetStringInfoLength(aes_info->key));
  (void) memcpy(datum,GetStringInfoDatum(key),MagickMin(
    GetStringInfoLength(key),GetStringInfoLength(aes_info->key)));
  for (i=0; i < n; i++)
    aes_info->encipher_key[i]=(unsigned int) datum[4*i] |
      ((unsigned int) datum[4*i+1] << 8) |
      ((unsigned int) datum[4*i+2] << 16) |
      ((unsigned int) datum[4*i+3] << 24);
  beta=1;
  bytes=(AESBlocksize/4)*(aes_info->rounds+1);
  for (i=n; i < bytes; i++)
  {
    alpha=aes_info->encipher_key[i-1];
    if ((i % n) == 0)
      {
        alpha=ByteSubTransform(RotateRight(alpha),SBox) ^ beta;
        beta=XTime((unsigned char) (beta & 0xff));
      }
    else
      if ((n > 6) && ((i % n) == 4))
        alpha=ByteSubTransform(alpha,SBox);
    aes_info->encipher_key[i]=aes_info->encipher_key[i-n] ^ alpha;
  }
  /*
    Generate deciper key (in reverse order).
  */
  for (i=0; i < 4; i++)
  {
    aes_info->decipher_key[i]=aes_info->encipher_key[i];
    aes_info->decipher_key[bytes-4+i]=aes_info->encipher_key[bytes-4+i];
  }
  for (i=4; i < (bytes-4); i+=4)
    InverseAddRoundKey(aes_info->encipher_key+i,aes_info->decipher_key+i);
  /*
    Reset registers.
  */
  datum=GetStringInfoDatum(aes_info->key);
  (void) memset(datum,0,GetStringInfoLength(aes_info->key));
  alpha=0;
  beta=0;
}