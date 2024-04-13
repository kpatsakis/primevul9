_gnutls_tls_create_random (opaque * dst)
{
  uint32_t tim;
  int ret;

  /* Use weak random numbers for the most of the
   * buffer except for the first 4 that are the
   * system's time.
   */

  tim = time (NULL);
  /* generate server random value */
  _gnutls_write_uint32 (tim, dst);

  ret = _gnutls_rnd (RND_NONCE, &dst[4], TLS_RANDOM_SIZE - 4);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return 0;
}