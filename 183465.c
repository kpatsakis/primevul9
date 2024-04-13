_gnutls_copy_ciphersuites (gnutls_session_t session,
			   opaque * ret_data, size_t ret_data_size)
{
  int ret, i;
  cipher_suite_st *cipher_suites;
  uint16_t cipher_num;
  int datalen, pos;

  ret = _gnutls_supported_ciphersuites_sorted (session, &cipher_suites);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  /* Here we remove any ciphersuite that does not conform
   * the certificate requested, or to the
   * authentication requested (eg SRP).
   */
  ret =
    _gnutls_remove_unwanted_ciphersuites (session, &cipher_suites, ret, -1);
  if (ret < 0)
    {
      gnutls_assert ();
      gnutls_free (cipher_suites);
      return ret;
    }

  /* If no cipher suites were enabled.
   */
  if (ret == 0)
    {
      gnutls_assert ();
      gnutls_free (cipher_suites);
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }

  cipher_num = ret;

  cipher_num *= sizeof (uint16_t);	/* in order to get bytes */

  datalen = pos = 0;

  datalen += sizeof (uint16_t) + cipher_num;

  if ((size_t) datalen > ret_data_size)
    {
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }

  _gnutls_write_uint16 (cipher_num, ret_data);
  pos += 2;

  for (i = 0; i < (cipher_num / 2); i++)
    {
      memcpy (&ret_data[pos], cipher_suites[i].suite, 2);
      pos += 2;
    }
  gnutls_free (cipher_suites);

  return datalen;
}