static bool encrypt_des(const unsigned char *in, unsigned char *out,
                        const unsigned char *key_56)
{
  char key[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = sizeof(key);

  /* Expand the 56-bit key to 64-bits */
  extend_key_56_to_64(key_56, ctl.Crypto_Key);

  /* Set the key parity to odd */
  Curl_des_set_odd_parity((unsigned char *) ctl.Crypto_Key, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *) &out, &ctl, (_SPCPTR *) &in);

  return TRUE;
}