gnutls_session_set_data (gnutls_session_t session,
                         const void *session_data, size_t session_data_size)
{
  int ret;
  gnutls_datum_t psession;

  psession.data = (opaque *) session_data;
  psession.size = session_data_size;

  if (session_data == NULL || session_data_size == 0)
    {
      gnutls_assert ();
      return GNUTLS_E_INVALID_REQUEST;
    }
  ret = _gnutls_session_unpack (session, &psession);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return 0;
}