static bool encrypt_des(const unsigned char *in, unsigned char *out,
                        const unsigned char *key_56)
{
  char key[8];
  size_t out_len;
  CCCryptorStatus err;

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, key);

  /* Set the key parity to odd */
  Curl_des_set_odd_parity((unsigned char *) key, sizeof(key));

  /* Perform the encryption */
  err = CCCrypt(kCCEncrypt, kCCAlgorithmDES, kCCOptionECBMode, key,
                kCCKeySizeDES, NULL, in, 8 /* inbuflen */, out,
                8 /* outbuflen */, &out_len);

  return err == kCCSuccess;
}