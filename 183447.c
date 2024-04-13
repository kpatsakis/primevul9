_gnutls_client_set_comp_method (gnutls_session_t session, opaque comp_method)
{
  int comp_methods_num;
  uint8_t *compression_methods;
  int i;

  comp_methods_num = _gnutls_supported_compression_methods (session,
							    &compression_methods);
  if (comp_methods_num < 0)
    {
      gnutls_assert ();
      return comp_methods_num;
    }

  for (i = 0; i < comp_methods_num; i++)
    {
      if (compression_methods[i] == comp_method)
	{
	  comp_methods_num = 0;
	  break;
	}
    }

  gnutls_free (compression_methods);

  if (comp_methods_num != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNKNOWN_COMPRESSION_ALGORITHM;
    }

  session->internals.compression_method =
    _gnutls_compression_get_id (comp_method);


  return 0;
}