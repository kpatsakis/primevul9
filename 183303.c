set_key_iv_pbes2 (gcry_cipher_hd_t chd, char *salt, size_t saltlen, int iter,
                  const void *iv, size_t ivlen, const char *pw, int algo)
{
  unsigned char *keybuf;
  size_t keylen;
  int rc;

  keylen = gcry_cipher_get_algo_keylen (algo);
  if (!keylen)
    return -1;
  keybuf = gcry_malloc_secure (keylen);
  if (!keybuf)
    return -1;

  rc = gcry_kdf_derive (pw, strlen (pw),
                        GCRY_KDF_PBKDF2, GCRY_MD_SHA1,
                        salt, saltlen, iter, keylen, keybuf);
  if (rc)
    {
      log_error ("gcry_kdf_derive failed: %s\n", gpg_strerror (rc));
      gcry_free (keybuf);
      return -1;
    }

  rc = gcry_cipher_setkey (chd, keybuf, keylen);
  gcry_free (keybuf);
  if (rc)
    {
      log_error ("gcry_cipher_setkey failed: %s\n", gpg_strerror (rc));
      return -1;
    }


  rc = gcry_cipher_setiv (chd, iv, ivlen);
  if (rc)
    {
      log_error ("gcry_cipher_setiv failed: %s\n", gpg_strerror (rc));
      return -1;
    }
  return 0;
}