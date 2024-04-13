gnutls_session_get_data2 (gnutls_session_t session, gnutls_datum_t * data)
{

  int ret;

  if (data == NULL)
    {
      return GNUTLS_E_INVALID_REQUEST;
    }

  if (session->internals.resumable == RESUME_FALSE)
    return GNUTLS_E_INVALID_SESSION;

  ret = _gnutls_session_pack (session, data);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return 0;
}