void Curl_ntlm_core_lm_resp(const unsigned char *keys,
                            const unsigned char *plaintext,
                            unsigned char *results)
{
#ifdef USE_OPENSSL
  DES_key_schedule ks;

  setup_des_key(keys, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) results,
                  DESKEY(ks), DES_ENCRYPT);

  setup_des_key(keys + 7, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) (results + 8),
                  DESKEY(ks), DES_ENCRYPT);

  setup_des_key(keys + 14, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) (results + 16),
                  DESKEY(ks), DES_ENCRYPT);
#elif defined(USE_GNUTLS_NETTLE)
  struct des_ctx des;
  setup_des_key(keys, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_key(keys + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_key(keys + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#elif defined(USE_GNUTLS)
  gcry_cipher_hd_t des;

  gcry_cipher_open(&des, GCRY_CIPHER_DES, GCRY_CIPHER_MODE_ECB, 0);
  setup_des_key(keys, &des);
  gcry_cipher_encrypt(des, results, 8, plaintext, 8);
  gcry_cipher_close(des);

  gcry_cipher_open(&des, GCRY_CIPHER_DES, GCRY_CIPHER_MODE_ECB, 0);
  setup_des_key(keys + 7, &des);
  gcry_cipher_encrypt(des, results + 8, 8, plaintext, 8);
  gcry_cipher_close(des);

  gcry_cipher_open(&des, GCRY_CIPHER_DES, GCRY_CIPHER_MODE_ECB, 0);
  setup_des_key(keys + 14, &des);
  gcry_cipher_encrypt(des, results + 16, 8, plaintext, 8);
  gcry_cipher_close(des);
#elif defined(USE_NSS) || defined(USE_MBEDTLS) || defined(USE_DARWINSSL) \
  || defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, keys);
  encrypt_des(plaintext, results + 8, keys + 7);
  encrypt_des(plaintext, results + 16, keys + 14);
#endif
}