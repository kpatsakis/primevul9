set_key_iv (gcry_cipher_hd_t chd, char *salt, size_t saltlen, int iter,
            const char *pw, int keybytes)
{
  unsigned char keybuf[24];
  int rc;

  assert (keybytes == 5 || keybytes == 24);
  if (string_to_key (1, salt, saltlen, iter, pw, keybytes, keybuf))
    return -1;
  rc = gcry_cipher_setkey (chd, keybuf, keybytes);
  if (rc)
    {
      log_error ( "gcry_cipher_setkey failed: %s\n", gpg_strerror (rc));
      return -1;
    }

  if (string_to_key (2, salt, saltlen, iter, pw, 8, keybuf))
    return -1;
  rc = gcry_cipher_setiv (chd, keybuf, 8);
  if (rc)
    {
      log_error ("gcry_cipher_setiv failed: %s\n", gpg_strerror (rc));
      return -1;
    }
  return 0;
}