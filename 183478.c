_gnutls_copy_comp_methods (gnutls_session_t session,
			   opaque * ret_data, size_t ret_data_size)
{
  int ret, i;
  uint8_t *compression_methods, comp_num;
  int datalen, pos;

  ret = _gnutls_supported_compression_methods (session, &compression_methods);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  comp_num = ret;

  datalen = pos = 0;
  datalen += comp_num + 1;

  if ((size_t) datalen > ret_data_size)
    {
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }

  ret_data[pos++] = comp_num;	/* put the number of compression methods */

  for (i = 0; i < comp_num; i++)
    {
      ret_data[pos++] = compression_methods[i];
    }

  gnutls_free (compression_methods);

  return datalen;
}