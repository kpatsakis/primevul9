static void setup_des_key(const unsigned char *key_56,
                          gcry_cipher_hd_t *des)
{
  char key[8];

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, key);

  /* Set the key parity to odd */
  Curl_des_set_odd_parity((unsigned char *) key, sizeof(key));

  /* Set the key */
  gcry_cipher_setkey(*des, key, sizeof(key));
}