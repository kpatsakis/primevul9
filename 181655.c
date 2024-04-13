static bool encrypt_des(const unsigned char *in, unsigned char *out,
                        const unsigned char *key_56)
{
  mbedtls_des_context ctx;
  char key[8];

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, key);

  /* Set the key parity to odd */
  mbedtls_des_key_set_parity((unsigned char *) key);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned char *) key);
  return mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}