crypt_block (unsigned char *buffer, size_t length, char *salt, size_t saltlen,
             int iter, const void *iv, size_t ivlen,
             const char *pw, int cipher_algo, int encrypt)
{
  gcry_cipher_hd_t chd;
  int rc;

  rc = gcry_cipher_open (&chd, cipher_algo, GCRY_CIPHER_MODE_CBC, 0);
  if (rc)
    {
      log_error ( "gcry_cipher_open failed: %s\n", gpg_strerror(rc));
      wipememory (buffer, length);
      return;
    }

  if (cipher_algo == GCRY_CIPHER_AES128
      ? set_key_iv_pbes2 (chd, salt, saltlen, iter, iv, ivlen, pw, cipher_algo)
      : set_key_iv (chd, salt, saltlen, iter, pw,
                    cipher_algo == GCRY_CIPHER_RFC2268_40? 5:24))
    {
      wipememory (buffer, length);
      goto leave;
    }

  rc = encrypt? gcry_cipher_encrypt (chd, buffer, length, NULL, 0)
              : gcry_cipher_decrypt (chd, buffer, length, NULL, 0);

  if (rc)
    {
      wipememory (buffer, length);
      log_error ( "en/de-crytion failed: %s\n", gpg_strerror (rc));
      goto leave;
    }

 leave:
  gcry_cipher_close (chd);
}